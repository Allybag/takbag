#include "tak.h"

#include <iostream>

int main()
{
    Game game(6);
    game.play("a1");
    game.play("a6");
    game.play("c3");
    game.play("c4");

    game.play("c3>");
    game.play("a6-");

    game.play("Cc3");
    game.play("Sc6");
    game.play("c3>");
    game.play("Sa6");
    game.play("3c4>21C");

    std::cout << game.print();
    return 0;
}
