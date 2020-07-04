#pragma once

#include <iostream>

#define LOG_INFO logging::LogInfo(__FILE__, __LINE__)
#define LOG_DEBUG logging::LogDebug(__FILE__, __LINE__)

using std::endl;

namespace logging {

    std::ostream& LogDebug(const char * file, int line);

    std::ostream& LogInfo(const char * file, int line);

} // namespace logging