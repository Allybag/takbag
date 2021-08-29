#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"

#include "boost/ut.hpp"
#pragma clang diagnostic pop

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
}
