#include "RootLogger.h"

#include <ctime>
#include <sys/time.h>
#include <iostream>
#include <iomanip>
#include <sstream>

void RootLogger::log(LogLevel logLevel, const std::string& message, const std::string& funcName, const std::string& logName)
{
    struct timeval epochTime;
    gettimeofday(&epochTime, nullptr);
    auto now = std::localtime(&epochTime.tv_sec);

    std::stringstream logLine;
    logLine << std::setfill('0') << std::setw(2);
    using std::setw;
    logLine << setw(2) << now->tm_hour << ":" << setw(2) << now->tm_min << ":" << setw(2) << now->tm_sec;
    logLine << "." << setw(6) << epochTime.tv_usec << " ";

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
