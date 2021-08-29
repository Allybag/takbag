#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include <set>

#include "tak/Position.h"
#include "tak/Game.h" // Game is basically the interface to Position

#include "utility.h"

// Borrowing this idea from https://github.com/MortenLohne/tiltak/
std::size_t fullShiftingPerft(const Position& position, std::size_t depth)
{
    std::set<std::size_t> results;
    for (Shift shiftType : shifts)
    {
        auto shiftedPos = position.shift(shiftType);
        results.insert(perft(shiftedPos, depth));
    }

    boost::ut::expect(results.size() == 1);
    return *results.begin();
}

int main()
{
    using namespace boost::ut;

    // These fullShiftingPerft tests and results are taken from github.com/MortenLohne/tiltak
    "Blank 5s Perft test"_test = []
    {
        Game game(5);
        Position pos = game.getPosition();

        expect(fullShiftingPerft(pos, 1) == 25);
        expect(fullShiftingPerft(pos, 2) == 600);
        expect(fullShiftingPerft(pos, 3) == 43'320);
        expect(fullShiftingPerft(pos, 4) == 2'999'784);
    };

    "Central Stack Perft"_test = []
    {
        Game game(5);
        std::vector<std::string> moves = {"d3", "c3", "c4", "d3<", "c4-", "Sc4"};
        for (const auto& move : moves)
            game.play(move);

        Position pos = game.getPosition();
        expect(fullShiftingPerft(pos, 1) == 87);
        expect(fullShiftingPerft(pos, 2) == 6'155);
        expect(fullShiftingPerft(pos, 3) == 461'800);
    };

    "Cap Perft"_test = []
    {
        Game game(5);
        std::vector<std::string> moves = {"c2", "c3", "d3", "b3", "c4", "1c2+",
                                          "1d3<", "1b3>", "1c4-", "Cc2", "a1", "1c2+", "a2"};
        for (const auto& move : moves)
            game.play(move);

        Position pos = game.getPosition();
        expect(fullShiftingPerft(pos, 1) == 104);
        expect(fullShiftingPerft(pos, 2) == 7'743);
        expect(fullShiftingPerft(pos, 3) == 592'645);
    };

    "Big Stack Perft"_test = []
    {
        Game game(5);
        std::vector<std::string> moves = {
                "c4", "c2", "d2", "c3", "b2", "d3", "1d2+", "b3", "d2", "b4", "1c2+", "1b3>", "2d3<",
                "1c4-", "d4", "5c3<23", "c2", "c4", "1d4<", "d3", "1d2+", "1c3+", "Cc3", "2c4>", "1c3<",
                "d2", "c3", "1d2+", "1c3+", "1b4>", "2b3>11", "3c4-12", "d2", "c4", "b4", "c5", "1b3>",
                "1c4<", "3c3-", "e5", "e2",
        };
        for (const auto& move : moves)
            game.play(move);

        Position pos = game.getPosition();
        expect(fullShiftingPerft(pos, 1) == 85);
        expect(fullShiftingPerft(pos, 2) == 11'206);
        expect(fullShiftingPerft(pos, 3) == 957'000);
    };
}