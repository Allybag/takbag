#include "Game.h"
#include <iostream>
#include "engine/Engine.h"

int main()
{
    Engine engine;

    Game game = Game(6);
    std::cout << game.print() << std::endl;

    std::string ptn;
    while (std::cin >> ptn && game.checkResult() == Result::None)
    {
        std::cout << "You play " << ptn << std::endl;
        game.play(ptn);
        std::cout << game.print() << std::endl;

        std::string engineMove = engine.chooseMove(game.getPosition());
        std::cout << "Engine plays " << engineMove << std::endl;
        game.play(engineMove);
        std::cout << game.print() << std::endl;
    }
}
