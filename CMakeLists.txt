cmake_minimum_required(VERSION 3.10)
project(ObjectDetector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
include(FetchContent)
set(ENV{GIT_SSL_NO_VERIFY} 1)


FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-30-2
)

FetchContent_MakeAvailable(asio)

set(ASIO_INCLUDE_DIR
    ${asio_SOURCE_DIR}/asio/include
    CACHE PATH "Asio include dir")

FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.3.2
)
FetchContent_MakeAvailable(crow)

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
target_link_libraries(detector ${OpenCV_LIBS} Crow::Crow)

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
