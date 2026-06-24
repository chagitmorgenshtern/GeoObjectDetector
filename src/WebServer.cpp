#include "WebServer.h"
#include "DetectionRequest.h"
#include <crow.h>
#include <fstream>
#include <iostream>

static bool fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

WebServer::WebServer(int port, std::shared_ptr<DetectionManager> detectionManager)
    : port(port), manager(std::move(detectionManager)), isRunning(false) {}

bool WebServer::start() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/detect").methods(crow::HTTPMethod::Post)([this](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, R"({"error": "Invalid JSON payload"})");
        }

        if (!body.has("image_path") || !body.has("top_left_lat") || !body.has("top_left_lon") || !body.has("resolution")) {
            return crow::response(400, R"({"error": "Missing required field"})");
        }

        DetectionRequest request;
        request.imagePath = body["image_path"].s();
        request.topLeftLat = body["top_left_lat"].d();
        request.topLeftLon = body["top_left_lon"].d();
        request.resolutionMetersPerPixel = body["resolution"].d();

        if (request.imagePath.empty()) {
            return crow::response(400, R"({"error": "image_path cannot be empty"})");
        }
        if (!fileExists(request.imagePath)) {
            return crow::response(400, R"({"error": "image_path does not exist"})");
        }
        if (request.resolutionMetersPerPixel <= 0.0) {
            return crow::response(400, R"({"error": "resolution must be positive"})");
        }

        try {
            std::vector<DetectionResult> results = manager->detect(request);
            std::string bodyOut = DetectionResult::arrayToJSON(results);
            crow::response res(200);
            res.set_header("Content-Type", "application/json");
            res.body = bodyOut;
            return res;
        } catch (const std::exception& ex) {
            crow::response res(500);
            res.set_header("Content-Type", "application/json");
            res.body = std::string("{\"error\": \"") + ex.what() + "\"}";
            return res;
        }
    });

    isRunning = true;
    std::cout << "Web server listening on port " << port << std::endl;
    app.port(port).multithreaded().run();
    isRunning = false;
    return true;
}

void WebServer::stop() {
    isRunning = false;
}

bool WebServer::isActive() const {
    return isRunning;
}
