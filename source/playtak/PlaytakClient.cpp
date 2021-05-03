#include "PlaytakClient.h"

#include "ServerMove.h"

#include <chrono>

static const std::string address = "playtak.com";
static const int port = 10'000;
const GameConfig defaultConfig = GameConfig{5, 600, 10, 0, 21, 1, false, false};

void PlaytakClient::ping()
{
    while (connected())
    {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        if (connected())
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
    while (connected())
        receiveMessages();

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

PlaytakMessage PlaytakClient::parseMessage(const std::string& message)
{
    const auto tokens = split(message, ' ');

    auto command = tokens.front();

    // Long, beautiful if chain
    if (command == "Login") // Login or Register
        return PlaytakMessage(PlaytakMessageType::ReadyForLogin);
    else if (command == "Welcome") // Welcome name!
    {
        assert(tokens.size() == 2);
        auto username = tokens[1].substr(0, tokens[1].size() - 1); // Lop off the ! on the end
        mName = username;
        mLogger << LogLevel::Info << "Logged in with username " << mName << Flush;
        return PlaytakMessage(PlaytakMessageType::LoginSuccess, username);
    }
    else if (command == "Game") // Game Start 422766 5 Guest778 vs Guest779 black 600 0 21 1
    {
        assert(tokens[1] == "Start");
        mActiveGameId = std::stoi(tokens[2]);
        auto playtakColour = tokens[7];
        assert(playtakColour == "white" || playtakColour == "black");
        return PlaytakMessage(PlaytakMessageType::StartGame, playtakColour);
    }
    else if (command.starts_with("Game#"))
    {
        auto operation = tokens[1];
        if (operation == "P" || operation == "M") // A move
        {
            auto serverMove = join(tokens, ' ', 1, 0);
            return PlaytakMessage(PlaytakMessageType::GameUpdate, serverToPtn(serverMove));
        }
        return PlaytakMessage(PlaytakMessageType::Ack);
    }
    else if (command == "GameList")
    {
        auto operation = tokens[1];
        if (operation == "Add")
            return PlaytakMessage(PlaytakMessageType::AddGame); // TODO: Game details
        else if (operation == "Remove")
            return PlaytakMessage(PlaytakMessageType::RemoveGame); // TODO: Game details
        else
            assert(false);
    }
    else
        return PlaytakMessage(PlaytakMessageType::Ack); // Atm we don't care about most messages
}

std::vector<PlaytakMessage> PlaytakClient::receiveMessages()
{
    std::vector<PlaytakMessage> relevantMessages;
    {
        std::string response = mClient.receive();
        auto messages = split(response, 0xa);
        for (const auto& message : messages)
        {
            if (message.empty())
                continue;

            mLogger << LogLevel::Info << "Received message: " << message << Flush;
            auto playtakMessage = parseMessage(message);
            if (isRelevant(playtakMessage.mType))
                relevantMessages.push_back(playtakMessage);
        }
    }

    return relevantMessages;
}

bool PlaytakClient::connected()
{
    return (!mStopping && mClient.connected());
}

bool PlaytakClient::sendMove(const std::string& move)
{
    std::string data = "Game#";
    data.append(std::to_string(mActiveGameId));
    data.push_back(' ');
    data.append(ptnToServer(move));
    return send(data);
}

