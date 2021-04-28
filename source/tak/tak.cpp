#include <iostream>

#include "tei.h"
#include "cmdLine.h"
#include "playtak/PlaytakClient.h"
#include "log/Logger.h"

int main()
{
    PlaytakClient client;
    client.connect();
    client.stream();
    return 0;
}

