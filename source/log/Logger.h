#pragma once

#include "RootLogger.h"

#include <cstdint>
#include <utility>

#define Log(logLevel, message) mLogger.log(logLevel, message, __func__)

class Logger
{
    std::string mName;
    LogLevel mLogLevel;

public:
    explicit Logger(std::string name) : mName(std::move(name)), mLogLevel(rootLogger.getGlobalLogLevel()) { }
    explicit Logger(std::string name, LogLevel logLevel) : mName(std::move(name)), mLogLevel(logLevel) { }

    void log(LogLevel logLevel, const std::string& message, const::std::string& funcName)
    {
        if (logLevel == LogLevel::Always || logLevel >= mLogLevel)
            // rootLogger is declared in RootLogger.h and defined in RootLogger.cpp
            rootLogger.log(logLevel, message, funcName, mName);
    }
};
