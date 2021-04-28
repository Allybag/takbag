#pragma once

#include "LogLevel.h"

#include <string>

class RootLogger
{
    LogLevel mGlobalLogLevel {LogLevel::Info};

public:
    void log(LogLevel logLevel, const std::string& message, const std::string& funcName, const std::string& logName);

    LogLevel getGlobalLogLevel() { return mGlobalLogLevel; }
    void setGlobalLogLevel(LogLevel logLevel) { mGlobalLogLevel = logLevel; }
};

extern RootLogger rootLogger;
