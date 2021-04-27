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

    // Can't login until we receive the ready message below
    bool readyForLogin = false;
    std::string readyMessage = "Login or Register";
    while (!readyForLogin)
    {
        std::string response = mClient.receive();
        auto messages = split(response, 0xa);
        for (const auto& message : messages)
            if (message == readyMessage)
                readyForLogin = true;
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

void PlaytakClient::stream()
{
    while (!mStopping && mClient.connected())
    {
        std::string response = mClient.receive();
        auto messages = split(response, 0xa);
        for (const auto& message : messages)
            std::cout << message << std::endl;
    }
}

