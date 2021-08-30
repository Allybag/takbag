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
    const bool useTranspositionTable = true;
    rootLogger.setLogToStdOut(true);

    // If we aren't using transposition tables this fails due to template linker errors
    // TODO: sort out!
#if 0
    "Test Block Wins"_test = []
    {
        Engine<useTranspositionTable> engine;
        Game game(5);
        const int searchDepth = 5;

        std::string movesTillTinueStart = "b1 c1 c2 b2 c3 b3 d3 a3 d2 a2 d1 a1 b5 e1 c4";

        for (const auto& move : split(movesTillTinueStart, ' '))
        {
            // Not actually playing any of the engine moves, just populating the transposition tables
            auto engineMove = engine.chooseMove(game.getPosition(), 1e9, searchDepth);
            game.play(move);
        }
        std::cout << game.print() << std::endl;

        const int moveCountTillTinue = 4;
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
#endif
}
