#include "Game.h"
#include <iostream>

int main(int argc, char** argv)
{
    Game game = Game(6);
    std::cout << game.print() << std::endl;

    std::string ptn;
    while (std::cin >> ptn)
    {
        game.play(ptn);
        std::cout << game.print() << std::endl;
    }
}
