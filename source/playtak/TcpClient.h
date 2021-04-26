#pragma once

#include <iostream>
#include <string>

// A generic TCP Client, can connect to a socket, and send and receive text

class TcpClient
{
    char mReceiveBuffer[1024];
    int mSocket;

    bool hasData();
public:
    TcpClient();
    bool connect(const std::string& site, int port);
    bool send(const std::string& data);
    std::string receive();
};
