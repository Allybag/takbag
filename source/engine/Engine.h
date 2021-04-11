#pragma once

#include "../Position.h"

class Engine
{
public:
    Position play(Position& position);
};

Position Engine::play(Position& position)
{
    assert(position.checkResult() == Result::None);
    auto moves = position.generateMoves();
    auto move = moves.front();
    if (move.mType == MoveType::Place)
        position.place(move);
    else
        position.move(move);
    return position;
}