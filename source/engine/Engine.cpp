#include "Engine.h"
#include "tak/Position.h"
#include "MonteCarlo.h"

std::string Engine::chooseMove(const Position& position)
{
    return monteCarloTreeSearch(position);
    // return chooseMoveRandom(position);
}

std::string Engine::chooseMoveFirst(const Position& position)
{
    assert(position.checkResult() == Result::None);
    auto moves = position.generateMoves();
    auto move = moves.front();
    return moveToPtn(move, position.size());
}

std::string Engine::chooseMoveRandom(const Position& position)
{
    assert(position.checkResult() == Result::None);
    auto moves = position.generateMoves();
    auto randomMove = chooseRandomElement(moves);
    return moveToPtn(*randomMove, position.size());
}
