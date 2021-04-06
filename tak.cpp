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
#elif 0
    // ["d3", "c3", "c4", "1d3<", "1c4-", "Sc4"]
    Game game(5);
    game.play("d3");
    game.play("c3");
    game.play("c4");
    game.play("d3<");
    game.play("c4-");
    game.play("Sc4");
    std::cout << game.print() << std::endl;
    std::cout << game.moveCount() << std::endl;

#else
    // PtnFile ptnFile("/Users/ally/clones/takbag/SimmonAllyRyderCupMatch.ptn");
    PtnFile ptnFile("/Users/ally/clones/takbag/LongStackyGame.ptn");
    // PtnFile ptnFile("/Users/ally/clones/takbag/RoadWin.ptn");
    Game ptnGame(ptnFile);
    std::cout << ptnGame.print() << std::endl;
#endif
    return 0;
}
