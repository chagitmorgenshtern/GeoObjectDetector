cmake_minimum_required(VERSION 3.10)
project(ObjectDetector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    src/DetectionResult.cpp
    src/ObjectDetector.cpp
    src/GeoReferencer.cpp
    src/Config.cpp
    src/DetectionManager.cpp
    src/WebServer.cpp
)

add_executable(detector ${SOURCES})
target_link_libraries(detector ${OpenCV_LIBS})

# Build unit tests
add_executable(test_detector 
    test/test_detector.cpp
    src/GeoReferencer.cpp
    src/DetectionResult.cpp
)
target_link_libraries(test_detector ${OpenCV_LIBS})

# Enable testing
enable_testing()
add_test(NAME detector_tests COMMAND test_detector)
