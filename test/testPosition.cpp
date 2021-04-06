#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Position.h"
#include "Game.h" // Game is basically the interface to Position

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
