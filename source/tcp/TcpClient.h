#pragma once

#include "log/Logger.h"

#include <string>

// A generic TCP Client, can connect to a socket, and send and receive text

class TcpClient
{
    char mReceiveBuffer[1024];
    int mSocket;

    Logger mLogger{"TCP"};

    bool hasData();
public:
    TcpClient();
    ~TcpClient();

    bool connect(const std::string& site, int port);
    bool send(const std::string& data);
    std::string receive();

    bool connected() { return mSocket != -1;}
};
