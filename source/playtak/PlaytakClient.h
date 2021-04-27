#pragma once

#include "TcpClient.h"

#include <vector>
#include <cstdint>
#include <thread>

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

struct Seek
{
    GameConfig mConfig;
    std::string mSeekingPlayer;
    std::string mSeekedPlayer; // Empty string if it's an open seek
    Colour seekColour;
};

struct PlaytakGame
{
    std::size_t mGameId;
    std::string mWhitePlayer;
    std::string mBlackPlayer;
};

class PlaytakClient
{
    TcpClient mClient;
    std::thread mPingThread;

    // State of the Server
    std::vector<Seek> mSeeks;
    std::vector<PlaytakGame> mGames;
    std::size_t mOnlineCount;

    bool mStopping;
    bool send(const std::string& data);
    void ping();
public:
    bool connect();

    void stream();


};
