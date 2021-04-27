#include <iostream>

#include "tei.h"
#include "cmdLine.h"
#include "playtak/PlaytakClient.h"

int main()
{
    PlaytakClient client;
    client.connect();
    return 0;
}

