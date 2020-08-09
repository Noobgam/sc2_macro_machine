#include "LogInfo.h"

#include <sstream>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>

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
            using namespace std::chrono;

            // get current time
            auto now = system_clock::now();

            // get number of milliseconds for the current second
            // (remainder after division into seconds)
            auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

            // convert to std::time_t in order to convert to std::tm (broken time)
            auto timer = system_clock::to_time_t(now);

            // convert to broken time
            std::tm bt = *std::localtime(&timer);
            std::stringstream ss;
            ss << std::put_time(&bt, "%d-%m-%Y %H-%M-%S");
            ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
            return ss.str();
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
