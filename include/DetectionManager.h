#pragma once

#include <vector>
#include <memory>
#include "ObjectDetector.h"
#include "GeoReferencer.h"
#include "DetectionResult.h"
#include "Config.h"

class DetectionManager {
private:
    std::unique_ptr<ObjectDetector> detector;
    std::unique_ptr<GeoReferencer> georeferencer;
    std::vector<DetectionResult> results;
    Config config;

public:
    DetectionManager(const Config& cfg);

    void processImage(const std::string& imagePath,
                      double topLeftLat,
                      double topLeftLon,
                      double resolutionMetersPerPixel);
    std::vector<DetectionResult> getResults() const;

    std::string resultsToJSON() const;
};
