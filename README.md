# Object Detector with Georeferencing

A C++ application that detects objects in satellite/aerial images and maps their bounding boxes to geographic coordinates (latitude/longitude).

## Architecture

### Core Classes

1. **ObjectDetector** — Detects objects in images using OpenCV contour detection
2. **GeoReferencer** — Converts pixel coordinates to lat/lon using calibration points
3. **DetectionResult** — Data structure holding object_id, bounding_box, and location
4. **DetectionManager** — Orchestrates the detection pipeline (detection → georeferencing → results)
5. **Config** — Loads and manages configuration (image path, calibration, server port)
6. **WebServer** — Serves JSON results via HTTP API

## Build

### Prerequisites

- CMake 3.10+
- OpenCV (with development libraries)
- C++17 compiler

### Compile

```bash
mkdir -p build
cd build
cmake ..
make
```

## Usage

Start the server:

```bash
./detector
```

Then send a single POST request to `/detect`:

```bash
curl -X POST http://localhost:8080/detect \
  -H 'Content-Type: application/json' \
  -d '{
    "image": "image.png",
    "geoCalibration": {
      "topLeft": {"lat": 32.0802, "lon": 34.7815},
      "topRight": {"lat": 32.0802, "lon": 34.7820},
      "bottomLeft": {"lat": 32.0797, "lon": 34.7815},
      "bottomRight": {"lat": 32.0797, "lon": 34.7820}
    },
    "resolution": {"width": 1920, "height": 1080}
  }'
```

The server will read the image from the local filesystem using the provided `image` file name.

Output: JSON array with detected objects, their bounding boxes, and geographic locations.

## Configuration

Create a `config.json` file with calibration points:

```json
{
  "image_path": "image.png",
  "geo_calibration": {
    "lat_top_left": 32.0802,
    "lon_top_left": 34.7815,
    "lat_top_right": 32.0803,
    "lon_top_right": 34.7820,
    "lat_bottom_left": 32.0797,
    "lon_bottom_left": 34.7815,
    "lat_bottom_right": 32.0797,
    "lon_bottom_right": 34.7820
  },
  "server_port": 8080,
  "detector_min_area": 500
}
```

## Output Example

```json
[
  {
    "object_id": 1,
    "bounding_box": [120, 300, 80, 60],
    "location": {
      "latitude": 32.0802,
      "longitude": 34.7815
    }
  }
]
```

## Next Steps

- [ ] Implement ObjectDetector (OpenCV contour detection)
- [ ] Implement GeoReferencer (pixel to lat/lon conversion)
- [ ] Complete DetectionManager orchestration
- [ ] Add WebServer HTTP endpoints
- [ ] Create test cases and sample image
