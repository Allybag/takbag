#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include "Position.h"
#include "Game.h" // Game is basically the interface to Position

int main()
{
    using namespace boost::ut;

    "Opening Moves"_test = []
    {
        Game game(6);
        expect(game.moveCount() == 36);

        game.play("a1");
        expect(game.moveCount() == 35); // Place a flat in any square other than a1

        game.play("c3");
        expect(game.moveCount() == 3 * 34 + 4); // Place a flat, wall or cap in any empty square, or move c3 any direction

        game.play("c3+");
        expect(game.moveCount() == 3 * 34 + 2); // Place a flat, wall or cap in any empty square, or move a1 up or right
    };

    "Basic Move Generation"_test = []
    {
        // This position and move count are taken from github.com/MortenLohne/tiltak
        // This is the only externally verified position and move count I've come across so far
        Game game(5);
        game.play("d3");
        game.play("c3");
        game.play("c4");
        game.play("d3<");
        game.play("c4-");
        game.play("Sc4");
        expect(game.moveCount() == 87);
    };

    "Basic Flat Win"_test = []
    {
        std::string ptnFile("games/FlatLoss.ptn"); // My first game which went to flats, loss against TakticianBot
        Game game = readGame(ptnFile);
        expect(game.checkResult() == Result::BlackFlat);
    };

    "Basic Board Fill"_test = []
    {
        std::string ptnFile("games/BoardFillDraw.ptn"); // My first game which went to flats, loss against TakticianBot
        Game game = readGame(ptnFile);
        expect(game.checkResult() == Result::Draw);
    };

    "Basic Road Win"_test = []
    {
        std::string ptnFile("games/RoadWin.ptn"); // My first game against a human on playtak
        Game game = readGame(ptnFile);
        expect(game.checkResult() == Result::BlackRoad);
    };

    "Dragon Road Win"_test = []
    {
        std::string ptnFile("games/DragonClause.ptn"); // A recent game with a dragon clause
        Game game = readGame(ptnFile);
        expect(game.checkResult() == Result::WhiteRoad);
    };
}
