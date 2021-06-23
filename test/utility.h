#pragma once

#include "tak/Position.h"
#include "engine/Engine.h"

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
        nodes += perft<checkWins>(nextPos, depth - 1);
    }

    return nodes;
}

std::string searchToDepth(Engine& engine, const Position& position, int depth)
{
    return engine.chooseMove(position, 1, depth);
}
