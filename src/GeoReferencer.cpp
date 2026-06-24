#include "GeoReferencer.h"
#include <stdexcept>

GeoReferencer::GeoReferencer()
    : imageWidth(0), imageHeight(0), isCalibrated(false) {}

void GeoReferencer::setCalibration(const CalibrationPoint& topLeft,
                                   const CalibrationPoint& topRight,
                                   const CalibrationPoint& bottomLeft,
                                   const CalibrationPoint& bottomRight) {
    calibrationPoints[0] = topLeft;
    calibrationPoints[1] = topRight;
    calibrationPoints[2] = bottomLeft;
    calibrationPoints[3] = bottomRight;
    isCalibrated = true;
}

void GeoReferencer::setImageDimensions(int width, int height) {
    imageWidth = width;
    imageHeight = height;
}

Location GeoReferencer::pixelToLatLon(double pixelX, double pixelY) {
    if (!isCalibrated) {
        throw std::runtime_error("GeoReferencer not calibrated");
    }
    if (imageWidth <= 0 || imageHeight <= 0) {
        throw std::runtime_error("Image dimensions not set");
    }

    double u = pixelX / static_cast<double>(imageWidth - 1);
    double v = pixelY / static_cast<double>(imageHeight - 1);

    const CalibrationPoint& tl = calibrationPoints[0];
    const CalibrationPoint& tr = calibrationPoints[1];
    const CalibrationPoint& bl = calibrationPoints[2];
    const CalibrationPoint& br = calibrationPoints[3];

    double latTop = tl.lat * (1 - u) + tr.lat * u;
    double latBottom = bl.lat * (1 - u) + br.lat * u;
    double lonTop = tl.lon * (1 - u) + tr.lon * u;
    double lonBottom = bl.lon * (1 - u) + br.lon * u;

    Location loc;
    loc.latitude = latTop * (1 - v) + latBottom * v;
    loc.longitude = lonTop * (1 - v) + lonBottom * v;
    return loc;
}
