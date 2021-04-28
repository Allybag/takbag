#include <iostream>

#include "tei.h"
#include "cmdLine.h"
#include "playtak/PlaytakClient.h"
#include "log/Logger.h"

int main()
{
    playCommandLine(5);
    return 0;

    PlaytakClient client;
    client.connect();
    client.stream();
    return 0;
}

