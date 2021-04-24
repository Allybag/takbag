#pragma once

#include "Game.h"
#include "engine/Engine.h"

#include <cstddef>
#include <string>
#include <iostream>

void playCommandLine(std::size_t size)
{
    Engine engine;

    Game game = Game(size);
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
