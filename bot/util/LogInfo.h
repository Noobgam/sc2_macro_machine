#pragma once

#include <iostream>

#define LOG_INFO logging::LogInfo(__FILE__, __LINE__)
#define LOG_DEBUG logging::LogDebug(__FILE__, __LINE__)

#ifdef _DEBUG
#define BOT_ENDL std::endl
#else
#define BOT_ENDL '\n'

#endif

namespace logging {

    void propagateFrame(int currentFrame);

    int getPropagatedFrame();

    std::ostream& LogDebug(const char * file, int line);

    std::ostream& LogInfo(const char * file, int line);

} // namespace logging