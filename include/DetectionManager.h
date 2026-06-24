#pragma once

#include <vector>
#include <memory>
#include "ObjectDetector.h"
#include "GeoReferencer.h"
#include "DetectionResult.h"
#include "Config.h"
#include "DetectionRequest.h"

class DetectionManager {
private:
    std::unique_ptr<ObjectDetector> detector;
    std::unique_ptr<GeoReferencer> georeferencer;
    Config config;

public:
    DetectionManager(const Config& cfg);

    std::vector<DetectionResult> processImage(const std::string& imagePath,
                      double topLeftLat,
                      double topLeftLon,
                      double resolutionMetersPerPixel);
    std::vector<DetectionResult> detect(const DetectionRequest& request);
};
