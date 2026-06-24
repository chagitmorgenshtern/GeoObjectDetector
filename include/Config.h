#pragma once

#include <string>
#include <map>

class Config {
public:
    std::string imagePath;
    int serverPort;
    int detectorMinArea;

    Config();
    bool loadFromFile(const std::string& configFile);
    bool loadDefaults();
};
