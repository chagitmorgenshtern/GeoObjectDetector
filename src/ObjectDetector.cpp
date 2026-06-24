#include "ObjectDetector.h"
#include <opencv2/opencv.hpp>
#include <stdexcept>

ObjectDetector::ObjectDetector() : minArea(500) {}

std::vector<BoundingBox> ObjectDetector::detectObjects(const std::string& imagePath) {
    cv::Mat image = cv::imread(imagePath);
    if (image.empty()) {
        throw std::runtime_error("Unable to open image: " + imagePath);
    }

    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);
    cv::Mat thresh;
    cv::adaptiveThreshold(gray, thresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY_INV, 11, 2);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<BoundingBox> boxes;
    for (const auto& contour : contours) {
        cv::Rect rect = cv::boundingRect(contour);
        if (rect.area() < minArea) {
            continue;
        }
        boxes.push_back({rect.x, rect.y, rect.width, rect.height});
    }

    return boxes;
}

void ObjectDetector::setMinArea(int area) {
    minArea = area;
}
