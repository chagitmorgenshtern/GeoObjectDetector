#pragma once

#include "DetectionResult.h"
#include <array>

class GeoReferencer {
public:
    struct CalibrationPoint {
        int pixel_x, pixel_y;
        double lat, lon;
    };

private:
    std::array<CalibrationPoint, 4> calibrationPoints;
    int imageWidth, imageHeight;
    bool isCalibrated;

public:
    GeoReferencer();

    void setCalibration(const CalibrationPoint& topLeft,
                        const CalibrationPoint& topRight,
                        const CalibrationPoint& bottomLeft,
                        const CalibrationPoint& bottomRight);

    void setImageDimensions(int width, int height);

    Location pixelToLatLon(double pixelX, double pixelY);
};
