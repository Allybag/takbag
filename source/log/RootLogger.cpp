#include "RootLogger.h"

#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>

void RootLogger::log(LogLevel logLevel, const std::string& message, const std::string& funcName, const std::string& logName)
{
    auto epochTime = std::time(nullptr);
    auto now = std::localtime(&epochTime);

    std::stringstream logLine;
    logLine << std::setfill('0') << std::setw(2);
    using std::setw;
    logLine << setw(2) << now->tm_hour << ":" << setw(2) << now->tm_min << ":" << setw(2) << now->tm_sec << " ";

    logLine << "[" << logLevelToStr(logLevel) << "]" << " ";
    logLine << "[" << logName << "]" << " ";
    logLine << "[" << funcName << "]" << " ";
    logLine << message;

    if (mLogToStdOut)
        std::cout << logLine.str() << std::endl;

    if (mLogFile.is_open())
        mLogFile << logLine.str() << std::endl;
}

RootLogger rootLogger;
