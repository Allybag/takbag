#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"

#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include "tak/Position.h"
#include "engine/Engine.h"
#include "tak/Game.h" // Game is basically the interface to Position

int main()
{
    using namespace boost::ut;

    "Reversible shifting"_test = []
    {
        Engine engine;
        Position position(6);

        for (int i = 0; i < 12; i++)
        {
            position.play(engine.chooseMoveRandom(position));
        }

        for (const auto shiftType : shifts)
        {
            auto shiftedPosition = position.shift(shiftType);
            auto originalPosition = shiftedPosition.shift(getReverseShift(shiftType));

            expect(originalPosition == position);
        }
    };

    "Opening book"_test = []
    {
        Engine noOpeningEngine;
        Engine engine("test/Openings.txt");

        Game game(6);
        // f1 is not in the file, but an equivalent by symmetry position (a1) is
        game.play("a6");

        expect(engine.openingBookContains(game.getPosition()));
        expect(!noOpeningEngine.openingBookContains(game.getPosition()));

        game.play("c3");
        expect(!engine.openingBookContains(game.getPosition()));

    };
}
