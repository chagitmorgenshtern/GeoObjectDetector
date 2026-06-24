#include "Config.h"

Config::Config()
    : imagePath("image.png"), serverPort(8080), detectorMinArea(500) {
    geoCalibration.lat_top_left = 0.0;
    geoCalibration.lon_top_left = 0.0;
    geoCalibration.lat_top_right = 0.0;
    geoCalibration.lon_top_right = 0.0;
    geoCalibration.lat_bottom_left = 0.0;
    geoCalibration.lon_bottom_left = 0.0;
    geoCalibration.lat_bottom_right = 0.0;
    geoCalibration.lon_bottom_right = 0.0;
}

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
