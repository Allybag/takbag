#pragma once

#include <cassert>
#include <cstdint>
#include <string>

enum class LogLevel : uint8_t
{
    Always,
    Warn,
    Error,
    Info,
    Debug,
    Trace,
    Unset,
};

inline std::string logLevelToStr(LogLevel logLevel)
{
    switch (logLevel)
    {
    case LogLevel::Always:
        return "ALWAYS";
    case LogLevel::Warn:
        return "WARN";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Trace:
        return "TRACE";
    case LogLevel::Unset:
        return "UNSET";
    }

    assert(false);
    return "";
}
