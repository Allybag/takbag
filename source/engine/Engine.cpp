#include "Engine.h"
#include "tak/Position.h"

std::string Engine::chooseMove(const Position& position)
{
    assert(position.checkResult() == Result::None);
    auto moves = position.generateMoves();
    auto move = moves.front();
    return moveToPtn(move, position.size());
}
