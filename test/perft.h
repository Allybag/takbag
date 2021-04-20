#pragma once

#include "tak/Position.h"

template <bool checkWins = true> // To let us independently time win checking and move generation
std::size_t perft(const Position& position, std::size_t depth)
{
    std::size_t nodes = 0;
    if (depth == 0 || (checkWins && position.checkResult() != Result::None))
        return 1;

    for (const auto& move : position.generateMoves())
    {
        Position nextPos(position);
        nextPos.play(move);
        nodes += perft(nextPos, depth - 1);
    }

    return nodes;
}

