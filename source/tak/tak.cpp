#include <iostream>

#include "tei.h"
#include "cmdLine.h"
#include "playtak/PlaytakClient.h"
#include "log/Logger.h"

struct LogTester
{
    Logger mLogger{"Test"};

    void bag() { Log(LogLevel::Info, "Hello"); }
};


int main()
{
    LogTester logTester;
    logTester.bag();
    return 0;

    PlaytakClient client;
    client.connect();
    client.stream();
    return 0;
}

