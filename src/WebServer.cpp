#include "WebServer.h"
#include "SimpleJson.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>

WebServer::WebServer(int port, std::shared_ptr<DetectionManager> detectionManager)
    : port(port), manager(std::move(detectionManager)), isRunning(false) {}

static std::string makeResponse(int statusCode, const std::string& statusText, const std::string& body) {
    std::ostringstream out;
    out << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
    out << "Content-Type: application/json\r\n";
    out << "Content-Length: " << body.size() << "\r\n";
    out << "Connection: close\r\n";
    out << "\r\n";
    out << body;
    return out.str();
}

static std::string readUntil(int sock, const std::string& delimiter) {
    std::string data;
    char buffer[1024];
    while (true) {
        ssize_t count = recv(sock, buffer, sizeof(buffer), 0);
        if (count <= 0) {
            break;
        }
        data.append(buffer, count);
        if (data.find(delimiter) != std::string::npos) {
            break;
        }
    }
    return data;
}

static bool readBytes(int sock, std::string& output, size_t count) {
    output.clear();
    output.reserve(count);
    while (output.size() < count) {
        char buffer[1024];
        size_t toRead = std::min(count - output.size(), sizeof(buffer));
        ssize_t received = recv(sock, buffer, toRead, 0);
        if (received <= 0) {
            return false;
        }
        output.append(buffer, static_cast<size_t>(received));
    }
    return true;
}

static std::string trim(const std::string& value) {
    size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }
    size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }
    return value.substr(start, end - start);
}

static std::string toLower(const std::string& value) {
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

static std::map<std::string, std::string> parseHeaders(const std::string& headerBlock) {
    std::map<std::string, std::string> headers;
    std::istringstream stream(headerBlock);
    std::string line;
    std::getline(stream, line);
    while (std::getline(stream, line)) {
        if (line.empty() || line == "\r") {
            continue;
        }
        size_t colon = line.find(':');
        if (colon == std::string::npos) {
            continue;
        }
        std::string name = trim(line.substr(0, colon));
        std::string value = trim(line.substr(colon + 1));
        headers[toLower(name)] = value;
    }
    return headers;
}

static std::string getHeaderValue(const std::map<std::string, std::string>& headers, const std::string& name) {
    auto it = headers.find(toLower(name));
    return it == headers.end() ? std::string() : it->second;
}

static std::string getPath(const std::string& requestLine) {
    std::istringstream stream(requestLine);
    std::string method, path, version;
    stream >> method >> path >> version;
    return path;
}

static std::string getMethod(const std::string& requestLine) {
    std::istringstream stream(requestLine);
    std::string method;
    stream >> method;
    return method;
}

static std::string getRequestLine(const std::string& requestData) {
    auto pos = requestData.find("\r\n");
    if (pos == std::string::npos) {
        return std::string();
    }
    return requestData.substr(0, pos);
}

static bool fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

bool WebServer::start() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket\n";
        return false;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        std::cerr << "Failed to bind socket to port " << port << "\n";
        close(serverSocket);
        return false;
    }

    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Listen failed\n";
        close(serverSocket);
        return false;
    }

    isRunning = true;
    std::cout << "Web server listening on port " << port << "\n";

    while (isRunning) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        if (clientSocket < 0) {
            continue;
        }

        std::string request = readUntil(clientSocket, "\r\n\r\n");
        if (request.empty()) {
            close(clientSocket);
            continue;
        }

        size_t headerEnd = request.find("\r\n\r\n");
        if (headerEnd == std::string::npos) {
            close(clientSocket);
            continue;
        }

        std::string headerBlock = request.substr(0, headerEnd + 2);
        std::string requestLine = getRequestLine(request);
        std::string path = getPath(requestLine);
        std::map<std::string, std::string> headers = parseHeaders(headerBlock);
        size_t contentLength = 0;
        std::string lengthValue = getHeaderValue(headers, "content-length");
        if (!lengthValue.empty()) {
            contentLength = std::stoul(lengthValue);
        }

        std::string body;
        size_t bodyStart = headerEnd + 4;
        if (bodyStart < request.size()) {
            body = request.substr(bodyStart);
        }
        if (contentLength > body.size()) {
            std::string remainder;
            if (!readBytes(clientSocket, remainder, contentLength - body.size())) {
                close(clientSocket);
                continue;
            }
            body += remainder;
        }

        std::string response;
        try {
            if (getMethod(requestLine) == "POST" && (path == "/detect" || path == "/detect/")) {
                SimpleJson::Value root = SimpleJson::parse(body);
                
                // Validate required fields
                if (!SimpleJson::hasKey(root, "image_path")) {
                    response = makeResponse(400, "Bad Request", "{\"error\": \"missing required field: image_path\"}");
                } else if (!SimpleJson::hasKey(root, "top_left_lat")) {
                    response = makeResponse(400, "Bad Request", "{\"error\": \"missing required field: top_left_lat\"}");
                } else if (!SimpleJson::hasKey(root, "top_left_lon")) {
                    response = makeResponse(400, "Bad Request", "{\"error\": \"missing required field: top_left_lon\"}");
                } else if (!SimpleJson::hasKey(root, "resolution")) {
                    response = makeResponse(400, "Bad Request", "{\"error\": \"missing required field: resolution\"}");
                } else {
                    std::string imagePath = SimpleJson::get(root, "image_path").string;
                    double topLeftLat = SimpleJson::get(root, "top_left_lat").number;
                    double topLeftLon = SimpleJson::get(root, "top_left_lon").number;
                    double resolution = SimpleJson::get(root, "resolution").number;
                    
                    // Validate values
                    if (imagePath.empty()) {
                        response = makeResponse(400, "Bad Request", "{\"error\": \"image_path cannot be empty\"}");
                    } else if (!fileExists(imagePath)) {
                        response = makeResponse(400, "Bad Request", "{\"error\": \"image_path does not exist\"}");
                    } else if (resolution <= 0) {
                        response = makeResponse(400, "Bad Request", "{\"error\": \"resolution must be positive\"}");
                    } else {
                        manager->processImage(imagePath, topLeftLat, topLeftLon, resolution);
                        std::string bodyOut = manager->resultsToJSON();
                        response = makeResponse(200, "OK", bodyOut);
                    }
                }
            } else {
                response = makeResponse(404, "Not Found", "{\"error\": \"Endpoint not found\"}");
            }
        } catch (const std::exception& ex) {
            std::ostringstream errorBody;
            errorBody << "{\"error\": \"" << ex.what() << "\"}";
            response = makeResponse(500, "Internal Server Error", errorBody.str());
        }

        send(clientSocket, response.c_str(), response.size(), 0);
        close(clientSocket);
    }

    close(serverSocket);
    return true;
}

void WebServer::stop() {
    isRunning = false;
}

bool WebServer::isActive() const {
    return isRunning;
}
