#pragma once

#include "LogLevel.h"

#include <string>
#include <fstream>
#include <iomanip>

class RootLogger
{
    bool mLogToStdOut{ false };
    LogLevel mGlobalLogLevel{ LogLevel::Info };
    std::ofstream mLogFile{ "tak.log" };

public:
    void log(LogLevel logLevel, const std::string& message, const std::string& funcName, const std::string& logName);

    LogLevel getGlobalLogLevel() { return mGlobalLogLevel; }
    void setGlobalLogLevel(LogLevel logLevel) { mGlobalLogLevel = logLevel; }
    void setLogToStdOut(bool value) { mLogToStdOut = value; }
};

extern RootLogger rootLogger;
