#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include "tak/Position.h"
#include "tak/Game.h" // Game is basically the interface to Position

#include "perft.h"

#include <chrono>

int main()
{
    using namespace boost::ut;
    using namespace std::chrono;

    "Perft 5s Opening Bench"_test = []
    {
        Game game(5);
        Position pos = game.getPosition();

        auto before = steady_clock::now();
        expect(perft(pos, 4) == 2'999'784);
        auto after = steady_clock::now();
        auto time = duration_cast<microseconds>(after - before);

        std::cout << "Checking four opening moves of a 5s game took " << time.count() << " mics" << std::endl;
    };

}