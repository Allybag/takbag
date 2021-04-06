#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Position.h"
#include "Game.h" // Game is basically the interface to Position

TEST_CASE("Test Opening Move Generation", "[moveGen]")
{
    Game game(6);
    REQUIRE(game.moveCount() == 36); // Place a flat in any square

    game.play("a1");
    REQUIRE(game.moveCount() == 35); // Place a flat in any square other than a1

    game.play("c3");
    REQUIRE(game.moveCount() == 3 * 34 + 4); // Place a flat, wall or cap in any empty square, or move c3 any direction

    game.play("c3+");
    REQUIRE(game.moveCount() == 3 * 34 + 2); // Place a flat, wall or cap in any empty square, or move a1 up or right
}

TEST_CASE("Test Basic Move Generation", "[moveGen]")
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
    REQUIRE(game.moveCount() == 87);
}

TEST_CASE("Test Basic Flat Win", "[gameResult]")
{
    PtnFile ptnFile("games/FlatLoss.ptn"); // My first game which went to flats, loss against TakticianBot
    Game game(ptnFile);
    REQUIRE(game.checkResult() == Result::BlackFlat);
}

TEST_CASE("Test Board Fill Flat Result", "[gameResult]")
{
    PtnFile ptnFile("games/BoardFillDraw.ptn");
    Game game(ptnFile);
    REQUIRE(game.checkResult() == Result::Draw);
}

TEST_CASE("Test Basic Road Win", "[gameResult]")
{
    PtnFile ptnFile("games/RoadWin.ptn"); // My first game against a human on playtak
    Game game(ptnFile);
    REQUIRE(game.checkResult() == Result::BlackRoad);
}

// TODO: Find the games to test this
#if 0
TEST_CASE("Test Road Supercedes Flat Win", "[gameResult]")
{
    PtnFile ptnFile("games/RoadWinLastFlat.ptn");
    Game game(ptnFile);
    REQUIRE(game.checkResult() == Result::BlackRoad);
}

TEST_CASE("Test Dragon Clause", "[gameResult]")
{
    PtnFile ptnFile("games/DragonClause.ptn");
    Game game(ptnFile);
    REQUIRE(game.checkResult() == Result::BlackRoad);
}
#endif
