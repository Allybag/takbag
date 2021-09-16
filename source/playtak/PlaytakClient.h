#pragma once

#include "log/Logger.h"
#include "tcp/TcpClient.h"

#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

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

enum class PlaytakMessageType : uint8_t
{
    ConnectionSuccess, // Welcome!
    ReadyForLogin,     // Login or Register
    LoginSuccess,      // Welcome Username!
    AddGame,           // GameList Add
    RemoveGame,        // GameList Remove
    StartGame,         // Game Start
    GameUpdate,        // Game#
    GameOver,          // Game# Over
    GameTime,          // Game# Time
    AddSeek,           // Seek Add
    RemoveSeek,        // Seek remove
    Observe,           // Observer
    GlobalChat,        // Shout
    ChatJoin,          // Joined
    ChatLeave,         // Left
    RoomChat,          // ShoutRoom
    PrivateChat,       // Tell
    PrivateChatSent,   // Told
    Announcement,      // Message
    Error,             // Error
    OnlineCount,       // Online
    Nak,               // NOK
    Ack,               // OK
};

inline bool isRelevant(PlaytakMessageType type)
{
    switch (type)
    {
    case PlaytakMessageType::StartGame:
    case PlaytakMessageType::GameUpdate:
    case PlaytakMessageType::GameOver:
    case PlaytakMessageType::GameTime:
        return true;

    case PlaytakMessageType::ConnectionSuccess:
    case PlaytakMessageType::ReadyForLogin:
    case PlaytakMessageType::LoginSuccess:
    case PlaytakMessageType::AddGame:
    case PlaytakMessageType::RemoveGame:
    case PlaytakMessageType::AddSeek:
    case PlaytakMessageType::RemoveSeek:
    case PlaytakMessageType::Observe:
    case PlaytakMessageType::GlobalChat:
    case PlaytakMessageType::ChatJoin:
    case PlaytakMessageType::ChatLeave:
    case PlaytakMessageType::RoomChat:
    case PlaytakMessageType::PrivateChat:
    case PlaytakMessageType::PrivateChatSent:
    case PlaytakMessageType::Announcement:
    case PlaytakMessageType::Error:
    case PlaytakMessageType::OnlineCount:
    case PlaytakMessageType::Nak:
    case PlaytakMessageType::Ack:
        return false;
    }
}

struct PlaytakMessage
{
    PlaytakMessageType mType;
    std::string mData;

    explicit PlaytakMessage(PlaytakMessageType type) : PlaytakMessage(type, "")
    {
    }
    PlaytakMessage(PlaytakMessageType type, std::string data) : mType(type), mData(std::move(data))
    {
    }
};

class PlaytakClient
{
    TcpClient mClient;
    std::thread mPingThread;
    Logger mLogger{"Playtak"};
    std::mutex mSendLock;

    // State of the Server
    std::vector<Seek> mSeeks;
    std::vector<PlaytakGame> mGames;
    std::size_t mOnlineCount;

    // Our state
    std::string mName;
    std::size_t mActiveGameId;

    bool mStopping;
    bool send(const std::string& data);
    void ping();

public:
    void stream();
    bool connect(const std::vector<std::string> = {});
    bool connected();
    bool sendMove(const std::string& move);
    std::vector<PlaytakMessage> receiveMessages();

    void seek(const GameConfig& gameConfig = defaultConfig, Colour colour = Colour::Any,
              const std::string opponent = "");
    void acceptSeek(std::size_t gameNum);

    PlaytakMessage parseMessage(const std::string& message);
};
