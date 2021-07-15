#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include "tak/Position.h"
#include "tak/Game.h" // Game is basically the interface to Position
#include "engine/Engine.h"
#include "other/StringOps.h"
#include "utility.h"

int main()
{
    using namespace boost::ut;

    "Test Negamax Search"_test = []
    {
        Engine engine;
        Game game(4);

        std::string movesTillTak = "d4 a1 a2 d3 a3";
        for (const auto& move : split(movesTillTak, ' '))
            game.play(move);

        // At depth 1, we can't do too much better than taking wins when we have them
        for (int depth = 1; depth <= 4; ++depth)
        {
            Game missedThreatGame(game);
            missedThreatGame.play("d2"); // A random move that doesn't block the win

            auto winningWhiteMove = searchToDepth(engine, missedThreatGame.getPosition(), depth);
            std::string onlyWinningWhiteMove = "a4";
            expect(winningWhiteMove == onlyWinningWhiteMove);

            missedThreatGame.play("b4"); // White misses the win...
            auto winningBlackMove = searchToDepth(engine, missedThreatGame.getPosition(), depth);
            std::string onlyWinningBlackMove = "d1";
            expect(winningBlackMove == onlyWinningBlackMove);
        }

        // At depth 2 we should block the enemy when they make a tak threat
        for (int depth = 2; depth <= 4; ++depth)
        {
            Game takGame(game);
            // At depth 2 we won't just let the opponent win immediately
            auto blackEngineMove = searchToDepth(engine, takGame.getPosition(), depth);
            checkEngineBlocksWin(blackEngineMove, takGame); // The engine should choose either Sa4 or just a4

            takGame.play("b1");
            takGame.play("d2");
            auto whiteEngineMove = searchToDepth(engine, takGame.getPosition(), depth);
            checkEngineBlocksWin(whiteEngineMove, takGame); // The engine should choose Sd1 or d1
        }

    };

    "Test Easy Tinue"_test = []
    {
        Engine engine;
        Game game(4);

        std::string movesTillFirstThreat = "a1 d4 b1 a2 c1 a3";
        for (const auto& move : split(movesTillFirstThreat, ' '))
            game.play(move);

        checkEngineBlocksWin(searchToDepth(engine, game.getPosition(), 3), game);

        std::string movesTillTinue = "1b1<1 b1 d1 b2 2a1>2 a1 a4 b4";
        for (const auto& move : split(movesTillTinue, ' '))
            game.play(move);

        checkEngineBlocksWin(searchToDepth(engine, game.getPosition(), 3), game);

        std::string onlyBlockingMove = "3b1<";
        game.play(onlyBlockingMove);

        std::string onlyTinueMove = "1a2-1";
        auto engineTinueMove = searchToDepth(engine, game.getPosition(), 3);
        expect(engineTinueMove == onlyTinueMove);
        game.play(engineTinueMove);

        std::string blockWesternRoad = "Sa2";
        game.play(blockWesternRoad);

        std::string onlyWinningMove = "4a1>112";
        auto engineWinningMove = searchToDepth(engine, game.getPosition(), 3);
        expect(engineWinningMove == onlyWinningMove);
    };
    "Test Avoid Suicide"_test = []
    {
        Engine engine;
        Game game(4);

        std::string movesTillSuicidable = "a1 d4 b1 a2 c1 a3 1b1<1 b1 d1 b1< a4 b3 b4 c4 1b4-1 b4";
        for (const auto& move : split(movesTillSuicidable, ' '))
            game.play(move);

        for (int depth = 2; depth <= 4; ++depth)
        {
            auto engineMove = searchToDepth(engine, game.getPosition(), depth);
            checkEngineBlocksWin(engineMove, game); // The engine should choose something like a4- or 2b3+
        }
    };

#ifndef LOW_MEMORY_COMPILE
    "Test Avoid Suicide 2"_test = []
    {
        Engine engine;
        Game game = readGame("games/GaveWin.ptn");

        for (int depth = 2; depth <= 4; ++depth)
        {
            auto engineMove = searchToDepth(engine, game.getPosition(), depth);
            checkEngineBlocksWin(engineMove, game); // The engine should choose something like a4- or 2b3+
        }
    };
#endif

    "Test Missed 6s Road"_test = []
    {
        Engine engine;
        Game game(6);

        std::string movesTillMissedRoad = "a1 f6 b2 a4 c2 Ca2 d2 a3 e2 a5 a6 b5 b6 c5 c6 d5";
        for (const auto& move : split(movesTillMissedRoad, ' '))
            game.play(move);

        game.play("e5");
        game.play("d6");
        expect(game.checkResult() != Result::None);
    };
}
