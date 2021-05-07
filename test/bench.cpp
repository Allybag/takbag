#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include "tak/Position.h"
#include "tak/Game.h" // Game is basically the interface to Position
#include "other/StringOps.h"
#include "other/Time.h"

#include "utility.h"

// Some of these functions will probably take ages if running unoptimised

int main()
{
    using namespace boost::ut;

    "Perft 6s Opening Bench"_test = []
    {
        Game game(6);
        Position pos = game.getPosition();

        auto before = timeInMics();
        expect(perft(pos, 4) == 13'586'048);
        auto after = timeInMics();
        auto duration = after - before;

        std::cout << "Checking four opening moves of a 6s game took " << duration << " mics" << std::endl;
    };

    "Depth 5 6s Opening Search Bench"_test = []
    {
        Game game(6);
        Engine engine;
        Position pos = game.getPosition();

        auto before = timeInMics();
        auto engineMove = searchToDepth(engine, pos, 5);
        auto after = timeInMics();
        auto duration = after - before;

        game.play(engineMove);
        std::cout << "Searching to depth 5 from open of a 6s game took " << duration << " mics" << std::endl;
    };

    "Stacky Perft"_test = []
    {
        Game game(5);
        std::string moves = {
                "a1 g7 d4 e5 e4 f4 d5 d3 Ce3 c3 c4 Cc5 b4 f3 e2 b3 d6 c5> c5 Cb5 c6 "
                "b5- e6 f5 f6 g5 b5 a3 e3< b6 a5 b6> Cb6 2c6- 2d3< 2b4> b4 b3+ e4+ "
                "Se7 b6- 2d5< 2b5- e4 Sd5 d3 c6 f7 g6 e7- b6 e3 d2 c2 d1 c1 b1 b2 b5 "
                "Sb3 e7 f2 d7 5c5+ c5 f1 f6+ f6 3c3> 3c4> b7 2e6+ c3 6c6-222 3b4> 3e7> "
                "4d3>13 d3 d2+ a2 e2+ d2 4f3+ e2 2d3- 4d4-13 5f4-14 5d2+14 Sf4 e4- 5f2<1112 "
                "3d4>21* 2e4-11 2d2>11 5e3>32 4e2+22 3b2> 2e3>11 5c2>113 4f3<13 5c4>14 g2 "
                "5e4- g1 4f2+ 2g3+ 5f3> c3> 2c3+11 4f7<1111 f3"
        };

        auto before = timeInMics();
        for (const auto& move : split(moves, ' '))
        {
            expect(perft(game.getPosition(), 3) > 0);
            game.play(move);
        }
        auto after = timeInMics();
        auto duration = after - before;

        std::cout << "Perft 3 of all moves of a stacky sevens game took " << duration << " mics" << std::endl;
    };
}