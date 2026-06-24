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
    "image_path": "/tmp/image.jpg",
    "top_left_lat": 32.1,
    "top_left_lon": 34.8,
    "resolution": 0.5
  }'
```

The server will read the image from the local filesystem using the provided `image_path`.

Output: JSON array with detected objects, their bounding boxes, and geographic locations.

## Assumptions

- **Image File Path**: The API receives a path to an image file (or an uploaded image file) and not the raw image bytes embedded in the request payload.
- **Image Resolution**: The image resolution represents the ground distance covered by a single pixel and is expressed in **meters per pixel**.

## Configuration

The server is currently configured with defaults:

- `server_port`: 8080
- `detector_min_area`: 500

You can override these values by editing `src/Config.cpp` or adding optional configuration support later.

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

## Testing

Run the unit tests:

```bash
cd build
./test_detector
```

Tests verify GeoReferencer coordinate conversion and DetectionResult JSON serialization.
