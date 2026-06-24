#include <iostream>
#include <cassert>
#include "GeoReferencer.h"
#include "DetectionResult.h"

void testGeoReferencer() {
    std::cout << "Testing GeoReferencer..." << std::endl;
    
    GeoReferencer geo;
    geo.setOrigin(32.1, 34.8);
    geo.setResolution(0.5);
    geo.setImageDimensions(640, 480);
    
    Location loc = geo.pixelToLatLon(0, 0);
    assert(loc.latitude == 32.1);
    assert(loc.longitude == 34.8);
    std::cout << "  ✓ Origin mapping correct\n";
    
    // Test a pixel offset
    Location loc2 = geo.pixelToLatLon(100, 100);
    assert(loc2.latitude < 32.1);
    assert(loc2.longitude > 34.8);
    std::cout << "  ✓ Pixel offset mapping correct\n";
}

void testDetectionResult() {
    std::cout << "Testing DetectionResult..." << std::endl;
    
    BoundingBox bbox{100, 200, 50, 60};
    Location loc{32.05, 34.85};
    DetectionResult result(1, bbox, loc);
    
    assert(result.object_id == 1);
    assert(result.bounding_box.x == 100);
    assert(result.location.latitude == 32.05);
    
    std::string json = result.toJSON();
    assert(json.find("\"object_id\": 1") != std::string::npos);
    assert(json.find("\"latitude\": 32.05") != std::string::npos);
    std::cout << "  ✓ DetectionResult JSON serialization correct\n";
}

int main() {
    std::cout << "Running unit tests...\n" << std::endl;
    
    try {
        testGeoReferencer();
        testDetectionResult();
        std::cout << "\n✅ All tests passed!\n" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
