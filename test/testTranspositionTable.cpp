#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include "tak/Position.h"
#include "tak/Game.h"
#include "engine/Engine.h"
#include "other/StringOps.h"
#include "utility.h"

int main()
{
    using namespace boost::ut;

    "Test Block Wins"_test = []
    {
        EngineOptions options;
        options.mUseTranspositionTable = true;

        Engine engine(options);
        Game game(4);
        const int searchDepth = 5;

        std::string movesTillTinueStart = "b1 c1 c2 b2 d2 d4 d1 a1 b4 a2";

        for (const auto& move : split(movesTillTinueStart, ' '))
        {
            // Not actually playing any of the engine moves, just populating the transposition tables
            auto engineMove = engine.chooseMove(game.getPosition(), 1e9, searchDepth);
            game.play(move);
        }
        std::cout << game.print() << std::endl;

        const int moveCountTillTinue = 3;
        for (int i = 0; i < moveCountTillTinue + 1; ++i)
        {
            auto engineMove = engine.chooseMove(game.getPosition(), 1e9, moveCountTillTinue + 2 - i);
            game.play(engineMove);
            std::cout << game.print() << std::endl;
            assert(game.checkResult() == Result::None);
        }

        auto winningMove = engine.chooseMove(game.getPosition(), 1e9, searchDepth);
        game.play(winningMove);
        expect(game.checkResult() == Result::WhiteRoad);
    };
}
