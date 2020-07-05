#include "FileUtils.h"

bool FileUtils::fileExists(const std::string &name) {
    auto&& stream = std::ifstream{"data/" + name};
    return stream.good();
}

std::ofstream FileUtils::openWrite(const std::string& name) {
    return std::ofstream{"data/" + name};
}

std::ifstream FileUtils::openRead(const std::string& name) {
    return std::ifstream{"data/" + name};
}
