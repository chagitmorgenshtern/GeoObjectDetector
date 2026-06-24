#pragma once

#include <vector>
#include <string>
#include "DetectionResult.h"

class ObjectDetector {
private:
    int minArea;

public:
    ObjectDetector();
    
    std::vector<BoundingBox> detectObjects(const std::string& imagePath);
    void setMinArea(int area);
};
