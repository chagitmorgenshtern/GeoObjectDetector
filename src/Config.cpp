#include "Config.h"

Config::Config()
    : imagePath("image.png"), serverPort(8080), detectorMinArea(500) {}

bool Config::loadFromFile(const std::string& configFile) {
    // Not implemented yet; defaults are enough for starting the server.
    return false;
}

bool Config::loadDefaults() {
    imagePath = "image.png";
    serverPort = 8080;
    detectorMinArea = 500;
    return true;
}
