#pragma once

#include "TcpClient.h"

#include <cstdint>

enum class Colour
{
    Any = 'A',
    White = 'W',
    Black = 'B'
};

struct GameConfig
{
    std::size_t mSize;
    std::size_t mTime;
    std::size_t mIncrement;
    std::size_t mKomi; // This is actually double the komi value
    std::size_t mFlatsCount;
    std::size_t mCapsCount;
    bool mIsRated;
    bool mIsTournament;
};

class PlaytakClient
{
    TcpClient mClient;
    bool send(const std::string& data);
public:
    bool connect();

};
