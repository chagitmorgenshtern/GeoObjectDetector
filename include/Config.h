#pragma once

#include <string>
#include <map>

struct GeoCalibration {
    double lat_top_left, lon_top_left;
    double lat_top_right, lon_top_right;
    double lat_bottom_left, lon_bottom_left;
    double lat_bottom_right, lon_bottom_right;
};

class Config {
public:
    std::string imagePath;
    GeoCalibration geoCalibration;
    int serverPort;
    int detectorMinArea;

    Config();
    bool loadFromFile(const std::string& configFile);
    bool loadDefaults();
};
