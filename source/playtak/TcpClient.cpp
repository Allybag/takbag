#include "TcpClient.h"

#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h> // addrinfo
#include <unistd.h>
#include <iostream>

TcpClient::TcpClient() : mSocket(-1)
{
    memset(mReceiveBuffer, 0, sizeof(mReceiveBuffer));
}

TcpClient::~TcpClient()
{
    if (mSocket != -1)
        close(mSocket);
}

bool TcpClient::connect(const std::string& site, int port)
{
    // create socket if it is not already created
    if (mSocket != -1)
    {
        std::cout << "Abandoning old connection" << std::endl;
        close(mSocket);
    }

    // This connection function is based on Beej's Network Programming Guide:
    // https://beej.us/guide/bgnet/
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    struct addrinfo* serverInfo;
    int status = getaddrinfo(site.c_str(), std::to_string(port).c_str(), &hints, &serverInfo);
    if (status != 0)
    {
        std::cout << "Address Info Disaster" << std::endl;
        return false;
    }

    mSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    if (mSocket == -1)
    {
        std::cout << "Socket creation disaster" << std::endl;
        return false;
    }

    int connected = ::connect(mSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
    if (connected == -1)
    {
        std::cout << "Connection disaster" << std::endl;
        return false;
    }

    freeaddrinfo(serverInfo); // We leak this if we fail at any stage above, but whatever
    std::cout << "Connected" << std::endl;
    return true;
}

bool TcpClient::send(const std::string& data) {
    auto sent = ::send(mSocket, data.c_str(), data.length(), 0);
    if (sent == data.length())
        return true;
    else if (sent <= 0)
        return false;

    std::cout << "TcpClient::send only sent " << sent << " bytes out of " << data.length() << std::endl;
    return true;
}

std::string TcpClient::receive()
{
    if (!hasData())
        return std::string();

    auto received = ::recv(mSocket, mReceiveBuffer, sizeof(mReceiveBuffer), 0);
    if (received <= 0)
    {
        mReceiveBuffer[0] = '\0';
        close(mSocket);
        mSocket = -1;
        std::cout << "Socket closed by server" << std::endl;
        return std::string();
    }
    mReceiveBuffer[received] = '\0';
    return std::string(mReceiveBuffer);
}

bool TcpClient::hasData()
{
    // A set of file descriptors, in our case just containing our socket
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(mSocket, &rfds);

    // We use a timeout of 100 millisconds
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100'000;

    return select(mSocket + 1, &rfds, nullptr, nullptr, &tv);
}

