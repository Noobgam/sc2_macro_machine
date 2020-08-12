#include "BotAssert.h"
#include "LogInfo.h"
#include <iostream>

namespace Assert
{
    std::string lastErrorMessage;

    std::string CurrentDateTime()
    {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::stringstream ss;
        ss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
        return ss.str();
    }

    void ReportFailure(const char * condition, const char * file, int line, const char * msg, ...)
    {
        char messageBuffer[1024] = "";
        if (msg != nullptr)
        {
            va_list args;
            va_start(args, msg);
            vsnprintf(messageBuffer, 1024, msg, args);
            va_end(args);
        }

        std::stringstream ss;
        ss                                              << BOT_ENDL;
        ss << "!Assert:   " << condition                << BOT_ENDL;
        ss << "File:      " << file                     << BOT_ENDL;
        ss << "Message:   " << messageBuffer            << BOT_ENDL;
        ss << "Line:      " << line                     << BOT_ENDL;
        ss << "Time:      " << CurrentDateTime()        << std::endl;
        
        lastErrorMessage = messageBuffer;

        std::cerr << ss.str();
    }
}

