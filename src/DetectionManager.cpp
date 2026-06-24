#include "DetectionManager.h"
#include <opencv2/opencv.hpp>
#include <stdexcept>

DetectionManager::DetectionManager(const Config& cfg)
    : detector(std::make_unique<ObjectDetector>()),
      georeferencer(std::make_unique<GeoReferencer>()),
      config(cfg) {
    detector->setMinArea(config.detectorMinArea);
}

void DetectionManager::processImage(const std::string& imagePath) {
    if (config.geoCalibration.lat_top_left == 0.0 &&
        config.geoCalibration.lon_top_left == 0.0 &&
        config.geoCalibration.lat_top_right == 0.0 &&
        config.geoCalibration.lon_top_right == 0.0 &&
        config.geoCalibration.lat_bottom_left == 0.0 &&
        config.geoCalibration.lon_bottom_left == 0.0 &&
        config.geoCalibration.lat_bottom_right == 0.0 &&
        config.geoCalibration.lon_bottom_right == 0.0) {
        throw std::runtime_error("Geo calibration not provided in config");
    }
    processImage(imagePath, config.geoCalibration, 0, 0);
}

void DetectionManager::processImage(const std::string& imagePath,
                                    const GeoCalibration& geoCalibration,
                                    int resolutionWidth,
                                    int resolutionHeight) {
    cv::Mat image = cv::imread(imagePath);
    if (image.empty()) {
        throw std::runtime_error("Unable to open image: " + imagePath);
    }

    if (resolutionWidth <= 0 || resolutionHeight <= 0) {
        resolutionWidth = image.cols;
        resolutionHeight = image.rows;
    }

    georeferencer->setImageDimensions(resolutionWidth, resolutionHeight);
    GeoReferencer::CalibrationPoint topLeft{0, 0, geoCalibration.lat_top_left, geoCalibration.lon_top_left};
    GeoReferencer::CalibrationPoint topRight{resolutionWidth - 1, 0, geoCalibration.lat_top_right, geoCalibration.lon_top_right};
    GeoReferencer::CalibrationPoint bottomLeft{0, resolutionHeight - 1, geoCalibration.lat_bottom_left, geoCalibration.lon_bottom_left};
    GeoReferencer::CalibrationPoint bottomRight{resolutionWidth - 1, resolutionHeight - 1, geoCalibration.lat_bottom_right, geoCalibration.lon_bottom_right};
    georeferencer->setCalibration(topLeft, topRight, bottomLeft, bottomRight);

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
