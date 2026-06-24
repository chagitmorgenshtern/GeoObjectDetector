#include "DetectionManager.h"
#include <opencv2/opencv.hpp>
#include <stdexcept>

DetectionManager::DetectionManager(const Config& cfg)
    : detector(std::make_unique<ObjectDetector>()),
      georeferencer(std::make_unique<GeoReferencer>()),
      config(cfg) {
    detector->setMinArea(config.detectorMinArea);
}

std::vector<DetectionResult> DetectionManager::processImage(const std::string& imagePath,
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
    std::vector<DetectionResult> results;
    int id = 1;
    for (const auto& box : boxes) {
        double centerX = box.x + box.width / 2.0;
        double centerY = box.y + box.height / 2.0;
        Location location = georeferencer->pixelToLatLon(centerX, centerY);
        results.emplace_back(id++, box, location);
    }
    return results;
}

std::vector<DetectionResult> DetectionManager::detect(const DetectionRequest& request) {
    return processImage(request.imagePath,
                        request.topLeftLat,
                        request.topLeftLon,
                        request.resolutionMetersPerPixel);
}
