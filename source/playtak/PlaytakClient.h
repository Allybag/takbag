#pragma once

#include "TcpClient.h"
#include "log/Logger.h"

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
    bool mIsPointless; // Nohat term for "Unrated"
    bool mIsTournament;
};

extern const GameConfig defaultConfig;

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
    Logger mLogger{"Playtak"};

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

    void seek(const GameConfig& gameConfig = defaultConfig, Colour colour = Colour::Any, const std::string opponent = "Ally");

};
