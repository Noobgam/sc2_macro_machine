#pragma once

#include <fstream>

class FileUtils {
public:
    static std::ofstream openWrite(const std::string& name);
    static std::ifstream openRead(const std::string& name);
    static bool fileExists(const std::string& name);
};


