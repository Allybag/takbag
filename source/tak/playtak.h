#pragma once

#include "log/Logger.h"
#include "playtak/PlaytakClient.h"
#include "Game.h"
#include "engine/Engine.h"

void playtak()
{
    Logger logger("player");
    PlaytakClient client;

    std::size_t gameSize = 6;
    client.connect();
    client.seek();
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
                client.seek();
            }

        }

    }
}
