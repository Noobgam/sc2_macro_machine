#pragma once

#include <string>
#include <fstream>

namespace FileUtils {
    bool createDirectories(std::string path);
    std::ofstream openWriteFile(std::string path);
    std::ifstream openReadFile(std::string path);
    bool fileExists(std::string path);
} // namespace FileUtils


