#pragma once

#include <string>

struct DetectionRequest {
    std::string imagePath;
    double topLeftLat = 0.0;
    double topLeftLon = 0.0;
    double resolutionMetersPerPixel = 0.0;
};
