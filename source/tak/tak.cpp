#include <iostream>

#include "tei.h"
#include "cmdLine.h"
#include "playtak/PlaytakClient.h"
#include "log/Logger.h"

int main()
{
    Logger logger("main");
    PlaytakClient client;

    client.connect();
    client.seek();
    Engine engine;
    Game game(5);
    while (client.connected())
    {
        auto messages = client.receiveMessages();
        for (const auto& message : messages)
        {
            if (message.mType == PlaytakMessageType::StartGame)
            {
                logger << LogLevel::Info << "Starting Game" << Flush;
                game = Game(5);
                if (message.mData == "white")
                {
                    auto engineMove = engine.chooseMove(game.getPosition());
                    logger << LogLevel::Info << "Sending move " << engineMove << Flush;
                    game.play(engineMove);
                    client.sendMove(engineMove);
                }
            }
            if (message.mType == PlaytakMessageType::GameUpdate)
            {
                auto opponentMove = message.mData;
                logger << LogLevel::Info << "Received move " << opponentMove << Flush;
                game.play(opponentMove);
                auto engineMove = engine.chooseMove(game.getPosition());
                logger << LogLevel::Info << "Sending move " << engineMove << Flush;
                game.play(engineMove);
                client.sendMove(engineMove);
            }

        }

    }
    return 0;
}

