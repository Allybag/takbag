#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include "tak/Position.h"
#include "tak/Game.h" // Game is basically the interface to Position
#include "engine/Engine.h"
#include "other/StringSplit.h"

void checkEngineBlocksWin(const std::string& engineMove, const Game& game)
{ // The move the engine plays should leave us unable to road on the next turn
    Game blockedGame = Game(game);
    blockedGame.play(engineMove);

    for (const auto& move : blockedGame.getPosition().generateMoves())
    {
        auto nextPosition = Position(blockedGame.getPosition());
        nextPosition.play(move);
        boost::ut::expect(nextPosition.checkResult() == Result::None);
    }
}

int main()
{
    using namespace boost::ut;

#if 1
    "Test Flat Counting"_test = []
    {
        // This isn't a great test, as it relies on the
        // "Flat Count is worth 2, flat on the board is worth 1" temporary scoring logic
        Engine engine;

        Game game(5);
        game.play("a1");
        expect(engine.evaluate(game.getPosition()) == -3); // Black is a flat ahead, wahey!

        game.play("e5");
        expect(engine.evaluate(game.getPosition()) == 0);

        game.play("e4");
        expect(engine.evaluate(game.getPosition()) == 3); // Black is a flat behind, booo!
    };

    "Test Negamax Search"_test = []
    {
        Engine engine;
        Game game(4);

        std::string movesTillTak = "d4 a1 a2 d3 a3";
        for (const auto& move : split(movesTillTak, ' '))
            game.play(move);

        // At depth 0, we can't do too much better than taking wins when we have them
        for (int depth = 0; depth <= 3; ++depth)
        {
            Game missedThreatGame(game);
            missedThreatGame.play("d2"); // A random move that doesn't block the win

            auto winningWhiteMove = engine.chooseMoveNegamax(missedThreatGame.getPosition(), depth);
            std::string onlyWinningWhiteMove = "a4";
            expect(winningWhiteMove == onlyWinningWhiteMove);

            missedThreatGame.play("b4"); // White misses the win...
            auto winningBlackMove = engine.chooseMoveNegamax(missedThreatGame.getPosition(), depth);
            std::string onlyWinningBlackMove = "d1";
            expect(winningBlackMove == onlyWinningBlackMove);
        }

        // At depth 1 we should block the enemy when they make a tak threat
        for (int depth = 1; depth <= 3; ++depth)
        {
            Game takGame(game);
            // At depth 1 we won't just let the opponent win immediately
            auto blackEngineMove = engine.chooseMoveNegamax(takGame.getPosition(), depth);
            checkEngineBlocksWin(blackEngineMove, takGame); // The engine should choose either Sa4 or just a4

            takGame.play("b1");
            takGame.play("d2");
            auto whiteEngineMove = engine.chooseMoveNegamax(takGame.getPosition(), depth);
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

        checkEngineBlocksWin(engine.chooseMoveNegamax(game.getPosition(), 3), game);

        std::string movesTillTinue = "1b1<1 b1 d1 b2 2a1>2 a1 a4 b4";
        for (const auto& move : split(movesTillTinue, ' '))
            game.play(move);

        checkEngineBlocksWin(engine.chooseMoveNegamax(game.getPosition(), 3), game);

        std::string onlyBlockingMove = "3b1<";
        game.play(onlyBlockingMove);

        std::string onlyTinueMove = "1a2-1";
        auto engineTinueMove = engine.chooseMoveNegamax(game.getPosition(), 3);
        expect(engineTinueMove == onlyTinueMove);
        game.play(engineTinueMove);

        std::string blockWesternRoad = "Sa2";
        game.play(blockWesternRoad);

        std::string onlyWinningMove = "4a1>112";
        auto engineWinningMove = engine.chooseMoveNegamax(game.getPosition(), 3);
        expect(engineWinningMove == onlyWinningMove);
    };
#endif
    "Test Avoid Suicide"_test = []
    {
        Engine engine;
        Game game(4);

        std::string movesTillSuicidable = "a1 d4 b1 a2 c1 a3 1b1<1 b1 d1 b1< a4 b3 b4 c4 1b4-1 b4";
        for (const auto& move : split(movesTillSuicidable, ' '))
            game.play(move);

        for (int depth = 1; depth <= 3; ++depth)
        {
            auto engineMove = engine.chooseMoveNegamax(game.getPosition(), depth);
            checkEngineBlocksWin(engineMove, game); // The engine should choose something like a4- or 2b3+
        }
    };
}
