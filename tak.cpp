#include "tak.h"

#include <iostream>

int main()
{
#if 0
    Game game(6);
    game.play("f6");
    game.play("a1");
    game.play("a2");
    game.play("f6-");
    game.play("a2+");
    game.play("f5<");
    game.play("a3>");

    std::cout << game.print();
    return 0;
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
#else

    // PtnFile ptnFile("/Users/ally/clones/takbag/RoadWin.ptn");
    PtnFile ptnFile("/Users/ally/clones/takbag/SimmonAllyRyderCupMatch.ptn");
    Game ptnGame(ptnFile);
#endif
    return 0;
}
