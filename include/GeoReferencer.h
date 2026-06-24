#pragma once

#include "DetectionResult.h"
#include <array>

class GeoReferencer {
private:
    double originLat;
    double originLon;
    double resolutionMetersPerPixel;
    int imageWidth;
    int imageHeight;
    bool hasOrigin;
    bool hasResolution;
    bool hasDimensions;

public:
    GeoReferencer();

    void setOrigin(double lat, double lon);
    void setResolution(double metersPerPixel);
    void setImageDimensions(int width, int height);

    Location pixelToLatLon(double pixelX, double pixelY) const;
};
