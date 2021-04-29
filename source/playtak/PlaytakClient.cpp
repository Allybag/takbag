#include "PlaytakClient.h"

#include "ServerMove.h"

#include <chrono>

static const std::string address = "playtak.com";
static const int port = 10'000;
const GameConfig defaultConfig = GameConfig{5, 600, 10, 0, 21, 1, false, false};

void PlaytakClient::ping()
{
    while (!mStopping && mClient.connected())
    {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        if (mClient.connected())
            send("PING");
    }

}
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

    mPingThread = std::thread(&PlaytakClient::ping, this);

    return true;
}

bool PlaytakClient::send(const std::string &data)
{
    // Shouldn't need a lock even though we call from two threads
    // as a single TCP send is atomic, and our messages should be very small
    std::string dataToSend(data);
    mLogger << LogLevel::Info << "Sending message: " << data << Flush;
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
            mLogger << LogLevel::Info << "Received message: " << message << Flush;
    }

    mPingThread.join();
}

void PlaytakClient::seek(const GameConfig& gameConfig, Colour colour, const std::string opponent)
{
    // Seek no time incr W/B/A komi pieces capstones unrated tournament opponent
    std::stringstream seekRequest;
    seekRequest << "Seek " << gameConfig.mSize << " ";
    seekRequest << gameConfig.mTime << " " << gameConfig.mIncrement << " ";
    seekRequest << static_cast<char>(colour) << " " << gameConfig.mKomi << " ";
    seekRequest << gameConfig.mFlatsCount << " " << gameConfig.mCapsCount << " ";
    seekRequest << gameConfig.mIsPointless << " " << gameConfig.mIsTournament << " ";

    if (!opponent.empty())
        seekRequest << opponent;

    send(seekRequest.str());

}

