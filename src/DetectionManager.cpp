#include "DetectionManager.h"
#include <opencv2/opencv.hpp>
#include <stdexcept>

DetectionManager::DetectionManager(const Config& cfg)
    : detector(std::make_unique<ObjectDetector>()),
      georeferencer(std::make_unique<GeoReferencer>()),
      config(cfg) {
    detector->setMinArea(config.detectorMinArea);
}

void DetectionManager::processImage(const std::string& imagePath,
                                      double topLeftLat,
                                      double topLeftLon,
                                      double resolutionMetersPerPixel) {
    cv::Mat image = cv::imread(imagePath);
    if (image.empty()) {
        throw std::runtime_error("Unable to open image: " + imagePath);
    }

    georeferencer->setOrigin(topLeftLat, topLeftLon);
    georeferencer->setResolution(resolutionMetersPerPixel);
    georeferencer->setImageDimensions(image.cols, image.rows);

    std::vector<BoundingBox> boxes = detector->detectObjects(imagePath);
    results.clear();
    int id = 1;
    for (const auto& box : boxes) {
        double centerX = box.x + box.width / 2.0;
        double centerY = box.y + box.height / 2.0;
        Location location = georeferencer->pixelToLatLon(centerX, centerY);
        results.emplace_back(id++, box, location);
    }
}

std::vector<DetectionResult> DetectionManager::getResults() const {
    return results;
}

std::string DetectionManager::resultsToJSON() const {
    std::string output = "[\n";
    for (size_t i = 0; i < results.size(); ++i) {
        output += results[i].toJSON();
        if (i + 1 < results.size()) {
            output += ",\n";
        }
    }
    output += "\n]\n";
    return output;
}
