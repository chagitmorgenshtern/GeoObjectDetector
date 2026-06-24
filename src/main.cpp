#include "Config.h"
#include "DetectionManager.h"
#include "WebServer.h"
#include <iostream>
#include <memory>

int main() {
    Config config;
    if (!config.loadDefaults()) {
        std::cerr << "Failed to load default configuration\n";
        return 1;
    }

    auto manager = std::make_shared<DetectionManager>(config);
    WebServer server(config.serverPort, manager);

    if (!server.start()) {
        std::cerr << "Failed to start server on port " << config.serverPort << "\n";
        return 1;
    }

    return 0;
}
