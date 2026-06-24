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

# For WebServer, we might need additional libraries like cpp-httplib or similar
# For now, leaving as placeholder
