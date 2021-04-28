#pragma once

#include "RootLogger.h"

#include <cstdint>
#include <utility>
#include <sstream>

struct WriteMessage
{
    std::string mFuncName;
    explicit WriteMessage(std::string funcName) : mFuncName(std::move(funcName)) { }
};

#define Flush WriteMessage(__func__)

class Logger
{
    std::string mName;
    LogLevel mLogLevel;

    // Current log line details
    LogLevel mActiveLevel;
    std::stringstream mActiveStream;

    bool shouldLog() { return (mActiveLevel == LogLevel::Always || mActiveLevel <= mLogLevel); }

public:
    explicit Logger(std::string name) : Logger(name, rootLogger.getGlobalLogLevel()) { }
    Logger(std::string name, LogLevel logLevel) : mName(std::move(name)), mLogLevel(logLevel), mActiveLevel(LogLevel::Unset) { }

    template <typename InputT>
    Logger& operator<<(InputT input)
    {
        assert(mActiveLevel != LogLevel::Unset);
        if (shouldLog())
            mActiveStream << input;
        return *this;
    }

    template <>
    Logger& operator<<(LogLevel logLevel)
    {
        assert(mActiveLevel == LogLevel::Unset);
        mActiveLevel = logLevel;
        return *this;
    }

    template <>
    Logger& operator<<(const WriteMessage flushMessage)
    {
        assert(mActiveLevel != LogLevel::Unset);

        // rootLogger is declared in RootLogger.h and defined in RootLogger.cpp
        if (shouldLog())
            rootLogger.log(mActiveLevel, mActiveStream.str(), flushMessage.mFuncName, mName);

        mActiveLevel = LogLevel::Unset;
        std::stringstream emptyStream;
        std::swap(mActiveStream, emptyStream);
        return *this;
    }
};
