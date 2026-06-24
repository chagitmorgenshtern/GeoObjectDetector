#include "DetectionResult.h"
#include <sstream>

DetectionResult::DetectionResult(int id, const BoundingBox& bbox, const Location& loc)
    : object_id(id), bounding_box(bbox), location(loc) {}

std::string DetectionResult::toJSON() const {
    std::ostringstream out;
    out << "  {\n";
    out << "    \"object_id\": " << object_id << ",\n";
    out << "    \"bounding_box\": [" << bounding_box.x << ", " << bounding_box.y << ", " << bounding_box.width << ", " << bounding_box.height << "],\n";
    out << "    \"location\": {\n";
    out << "      \"latitude\": " << location.latitude << ",\n";
    out << "      \"longitude\": " << location.longitude << "\n";
    out << "    }\n";
    out << "  }";
    return out.str();
}
