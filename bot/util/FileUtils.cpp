#include "FileUtils.h"
#include <boost/filesystem.hpp>

bool FileUtils::createDirectories(std::string path) {
    return boost::filesystem::create_directories(path);
}

std::ofstream FileUtils::openWriteFile(std::string path) {
    std::size_t found = path.find_last_of("/\\");
    if (found != std::string::npos) {
        createDirectories(path.substr(0, found));
    }
    return std::ofstream(path);
}

std::ifstream FileUtils::openReadFile(std::string path) {
    std::size_t found = path.find_last_of("/\\");
    if (found != std::string::npos) {
        createDirectories(path.substr(0, found));
    }
    return std::ifstream(path);
}

bool FileUtils::fileExists(std::string path) {
    return boost::filesystem::exists(path);
}