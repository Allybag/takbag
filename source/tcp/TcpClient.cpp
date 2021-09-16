#include "TcpClient.h"

#include <cstring> // memset for some reason
#include <netdb.h> // addrinfo
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

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
        mLogger << LogLevel::Info << "Abandoning old connection" << Flush;
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
        mLogger << LogLevel::Error << "Address Info Failure" << Flush;
        return false;
    }

    mSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    if (mSocket == -1)
    {
        mLogger << LogLevel::Error << "Socket Creation Failure" << Flush;
        return false;
    }

    int connected = ::connect(mSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
    if (connected == -1)
    {
        mLogger << LogLevel::Error << "Connection Failure" << Flush;
        return false;
    }

    freeaddrinfo(serverInfo); // We leak this if we fail at any stage above, but whatever
    mLogger << LogLevel::Info << "Connected" << Flush;
    return true;
}

bool TcpClient::send(const std::string& data)
{
    auto sent = ::send(mSocket, data.c_str(), data.length(), 0);
    if (sent == data.length())
        return true;
    else if (sent <= 0)
        return false;

    mLogger << LogLevel::Warn << "TcpClient::send only sent " << sent << " bytes out of " << data.length() << Flush;
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
        mLogger << LogLevel::Info << "Socket closed by server" << Flush;
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
