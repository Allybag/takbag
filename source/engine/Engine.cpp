#include "Engine.h"
#include "tak/Position.h"
#include "MonteCarlo.h"

int Engine::evaluate(const Position& position)
{
    auto result = position.checkResult();
    auto player = position.getPlayer();
    int colour = player == Player::White ? 1 : -1;

    if (result != Result::None)
    {
        if (result == Result::Draw)
            return 0;

        if ((player == Player::Black) == (result & StoneBits::Black))
            return 100 * colour; // Win

        return -100 * colour; // Loss
    }

    auto flatCounts = position.checkFlatCount();
    int score = 0;
    score += flatCounts[Player::White] * 2; // A top flat on the board is worth two points
    score -= flatCounts[Player::Black] * 2;

    auto reserveCounts = position.getReserveCount();
    score -= reserveCounts[Player::White]; // A flat on the board is worth one point
    score += reserveCounts[Player::Black];

    return score * colour;
}

std::string Engine::chooseMove(const Position& position)
{
    return chooseMoveNegamax(position);
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

std::string Engine::chooseMoveNegamax(const Position& position)
{
    auto moves = position.generateMoves();

    int bestScore = -101; // Worse than a loss
    Move* bestMove = nullptr;
    for (auto& move : moves)
    {
        auto nextPosition = Position(position);
        nextPosition.play(move);

        int score = -negamax(nextPosition, 3, -101, 101, -1);
        if (score > bestScore)
        {
            std::string ptnMove = moveToPtn(move, position.size());
            std::cout << ptnMove << " is new best move, with score " << score << std::endl;
            bestScore = score;
            bestMove = &move;
        }
    }

    return moveToPtn(*bestMove, position.size());
}

int Engine::negamax(const Position& position, int depth, int alpha, int beta, int colour)
{
    if (depth == 0 || position.checkResult() != Result::None)
        return evaluate(position) * colour;

    auto moves = position.generateMoves();
    // auto orderedMoves = orderMoves(moves);

    int score = -101;
    for (auto& move : moves)
    {
        Position nextPosition(position);
        nextPosition.play(move);

        int score = std::max(score, -negamax(nextPosition, depth - 1, beta * -1, alpha * -1, colour * -1));
        alpha = std::max(alpha, score);
        if (alpha >= beta)
           break; // Alpha Beta Cutoff Kapow!
    }

    return score;
}
