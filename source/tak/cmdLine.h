#pragma once

#include "Game.h"
#include "engine/Engine.h"

#include <cstddef>
#include <iostream>
#include <other/ArgParse.h>
#include <string>

void playCommandLine(const OptionMap& options)
{
    Engine engine;

    std::size_t gameSize = options.contains("size") ? std::stoi(options.at("size")) : 6;
    Game game = Game(gameSize);
    std::cout << game.print() << std::endl;

    std::string engineMove = engine.chooseMove(game.getPosition());
    std::cout << "Engine plays " << engineMove << std::endl;
    game.play(engineMove);
    std::cout << game.print() << std::endl;

    std::string ptn;
    while (std::cin >> ptn && game.checkResult() == Result::None)
    {
        std::cout << "You play " << ptn << std::endl;
        game.play(ptn);
        std::cout << game.print() << std::endl;

        if (game.checkResult() != Result::None)
            break;

        engineMove = engine.chooseMove(game.getPosition());
        std::cout << "Engine plays " << engineMove << std::endl;
        game.play(engineMove);
        std::cout << game.print() << std::endl;
    }
}
