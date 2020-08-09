#include "LogInfo.h"

#include <sstream>
#include <string>
#include <fstream>

namespace {

    static int frameId = -1;

    std::ostream& nullstream() {
        static std::ofstream os;
        static bool flag_set = false;
        if (!flag_set) {
            os.setstate(std::ios_base::badbit);
            flag_set = true;
        }
        return os;
    }

    std::string CurrentDateTime()
    {
        if (frameId == -1) {
            return "";
        } else {
            int seconds = frameId / 22.4;
            int minutes = seconds / 60;
            seconds -= minutes * 60;
            std::stringstream ss;
            ss << "[" << minutes << ":" << seconds << "]";
            return ss.str();
        }
    }
}

namespace logging {

    void propagateFrame(int currentFrame) {
        frameId = currentFrame;
    }

    std::ostream& LogDebug(const char * file, int line)
    {
#ifdef DEBUG_LOGGING
        return std::cerr << CurrentDateTime() << file << ':' << line << " - ";
#else
        return nullstream();
#endif
    }

    std::ostream& LogInfo(const char * file, int line)
    {
        return std::cerr << CurrentDateTime() << file << ':' << line << " - ";
    }

} // namespace logging
