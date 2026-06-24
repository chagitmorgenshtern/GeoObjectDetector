#pragma once

#include <memory>
#include "DetectionManager.h"

class WebServer {
private:
    int port;
    std::shared_ptr<DetectionManager> manager;
    bool isRunning;

public:
    WebServer(int port, std::shared_ptr<DetectionManager> detectionManager);

    bool start();
    void stop();
    bool isActive() const;
};
