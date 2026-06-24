#include "GeoReferencer.h"
#include <cmath>
#include <stdexcept>

GeoReferencer::GeoReferencer()
    : originLat(0.0), originLon(0.0), resolutionMetersPerPixel(0.0), imageWidth(0), imageHeight(0), hasOrigin(false), hasResolution(false), hasDimensions(false) {}

void GeoReferencer::setOrigin(double lat, double lon) {
    originLat = lat;
    originLon = lon;
    hasOrigin = true;
}

void GeoReferencer::setResolution(double metersPerPixel) {
    if (metersPerPixel <= 0.0) {
        throw std::runtime_error("Resolution must be positive");
    }
    resolutionMetersPerPixel = metersPerPixel;
    hasResolution = true;
}

void GeoReferencer::setImageDimensions(int width, int height) {
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Invalid image dimensions");
    }
    imageWidth = width;
    imageHeight = height;
    hasDimensions = true;
}

Location GeoReferencer::pixelToLatLon(double pixelX, double pixelY) const {
    if (!hasOrigin || !hasResolution || !hasDimensions) {
        throw std::runtime_error("GeoReferencer not configured");
    }

    // Calculate approximate latitude/longitude for pixel coordinates assuming north-up orientation.
    double metersPerDegreeLat = 111320.0;
    double metersPerDegreeLon = metersPerDegreeLat * std::cos(originLat * M_PI / 180.0);

    double northMeters = pixelY * resolutionMetersPerPixel;
    double eastMeters = pixelX * resolutionMetersPerPixel;

    Location loc;
    loc.latitude = originLat - northMeters / metersPerDegreeLat;
    loc.longitude = originLon + eastMeters / metersPerDegreeLon;
    return loc;
}
