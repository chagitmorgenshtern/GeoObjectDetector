#pragma once

#include <vector>
#include <string>

struct Location {
    double latitude;
    double longitude;
};

struct BoundingBox {
    int x, y, width, height;
};

class DetectionResult {
public:
    int object_id;
    BoundingBox bounding_box;
    Location location;

    DetectionResult(int id, const BoundingBox& bbox, const Location& loc);
    
    std::string toJSON() const;
};
