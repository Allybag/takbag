#include "PlaytakClient.h"

#include "ServerMove.h"

#include <iostream>

static const std::string address = "playtak.com";
static const int port = 10'000;

bool PlaytakClient::connect()
{
    if (!mClient.connect(address, port))
    {
        return false;
    }

    // We expect a message saying welcome
    std::string response = mClient.receive();

    // We can't Login until we get a message sayin
    std::string readyForLogin = "Login or Register\n";
    while (!response.ends_with(readyForLogin))
    {
        response = mClient.receive();
    }

    std::string loginCommand = "Login Guest";
    send(loginCommand);

    return true;
}

bool PlaytakClient::send(const std::string &data)
{
    std::string dataToSend(data);
    dataToSend.push_back(0xa); // New Line character that PlayTak.com treats as End Of Message
    return mClient.send(dataToSend);
}

