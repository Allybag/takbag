#pragma once

#include "../Position.h"

class Engine
{
public:
    std::string chooseMove(const Position& position);
};

std::string Engine::chooseMove(const Position& position)
{
    assert(position.checkResult() == Result::None);
    auto moves = position.generateMoves();
    auto move = moves.front();
    return moveToPtn(move, position.size());
}
