#include "Game.h"
#include <iostream>
#include "engine/Engine.h"

int main(int argc, char** argv)
{
    Engine engine;

    Game game = Game(6);
    std::cout << game.print() << std::endl;

    std::string ptn;
    while (std::cin >> ptn && game.checkResult() == Result::None)
    {
        game.play(ptn);
        std::cout << game.print() << std::endl;

        engine.play(game.getPosition());
        game.incrementPly();
        std::cout << game.print() << std::endl;
    }
}
