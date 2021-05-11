#pragma once

#include "log/Logger.h"
#include "playtak/PlaytakClient.h"
#include "Game.h"
#include "engine/Engine.h"

#include <vector>
#include <string>

void playtak(const OptionMap& options)
{
    Logger logger("player");
    PlaytakClient client;

    std::vector<std::string> loginDetails;
    if (options.contains("user"))
    {
        assert(options.contains"pass");
        loginDetails.push_back(options.at("user"));
        loginDetails.push_back(options.at("pass"));
    }
    client.connect(loginDetails);

    std::size_t gameSize = options.contains("size") ? std::stoi(options.at("size")) : 6;
    std::size_t flats = options.contains("flats") ? std::stoi(options.at("flats")) : pieceCounts[gameSize].first;
    std::size_t caps = options.contains("caps") ? std::stoi(options.at("flats")) : pieceCounts[gameSize].second;
    std::size_t time = options.contains("time") ? std::stoi(options.at("time")) : 180;
    std::size_t incr = options.contains("increment") ? std::stoi(options.at("increment")) : 5;
    std::size_t komi = options.contains("komi") ? std::stoi(options.at("komi")) : 0; // Units are half-komi

    GameConfig gameConfig {gameSize, time, incr, komi, flats, caps, false, false};
    client.seek(gameConfig);

    Engine engine;
    Game game(gameSize);
    int colour = 0;
    int remainingTime = 0;
    while (client.connected())
    {
        auto messages = client.receiveMessages();
        for (const auto& message : messages)
        {
            if (message.mType == PlaytakMessageType::StartGame)
            {
                logger << LogLevel::Info << "Starting Game" << Flush;
                game = Game(gameSize);
                if (message.mData == "white")
                {
                    colour = 0;
                    auto engineMove = engine.chooseMove(game.getPosition());
                    logger << LogLevel::Info << "Sending move " << engineMove << Flush;
                    game.play(engineMove);
                    client.sendMove(engineMove);
                }
                else
                {
                    colour = 1;
                }
            }
            else if (message.mType == PlaytakMessageType::GameUpdate)
            {
                auto opponentMove = message.mData;
                logger << LogLevel::Info << "Received move " << opponentMove << Flush;
                game.play(opponentMove);
                auto engineMove = engine.chooseMove(game.getPosition(), remainingTime / 10);
                logger << LogLevel::Info << "Sending move " << engineMove << Flush;
                game.play(engineMove);
                client.sendMove(engineMove);
            }
            else if (message.mType == PlaytakMessageType::GameTime)
            {
                auto remainingTimes = split(message.mData, ' ');
                remainingTime = std::stoi(remainingTimes[colour]);
            }
            else if (message.mType == PlaytakMessageType::GameOver)
            {
                logger << LogLevel::Info << "Game Over!" << Flush;
                client.seek(gameConfig);
            }

        }

    }
}
