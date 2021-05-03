#include "Engine.h"
#include "tak/Position.h"
#include "MonteCarlo.h"

#include <chrono>

static constexpr int winValue = 100;
static constexpr int infinity = 10001; // Not really infinity, but pretty high

// Returns a score from white's point of view
int Engine::evaluate(const Position& position)
{
    auto result = position.checkResult();

    if (result != Result::None)
    {
        if (result == Result::Draw)
            return 0;

        if (result & StoneBits::Black)
            return -winValue;

        return winValue;
    }

    auto flatCounts = position.checkFlatCount();
    int score = 0;
    score += static_cast<int>(flatCounts[Player::White]) * 2; // A top flat on the board is worth two points
    score -= static_cast<int>(flatCounts[Player::Black]) * 2;

    auto reserveCounts = position.getReserveCount();
    score -= reserveCounts[Player::White]; // A flat on the board is worth one point
    score += reserveCounts[Player::Black];

    return score;
}

std::string Engine::chooseMove(const Position& position)
{
    using namespace std::chrono;
    auto startTime = steady_clock::now();
    mStats.reset();

    auto move = chooseMoveNegamax(position, 3);

    auto stopTime = steady_clock::now();
    auto duration = duration_cast<microseconds>(stopTime - startTime);
    mLogger << LogLevel::Info << "Evaluated " << mStats.mNodeCount << " nodes in " << duration.count() << " mics" << Flush;
    return chooseMoveNegamax(position, 2);
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

std::string Engine::chooseMoveNegamax(const Position& position, int depth)
{
    auto moves = position.generateMoves();

    int colour = position.getPlayer() == Player::Black ? 1 : -1;
    int bestScore = -infinity;
    Move* bestMove = nullptr;
    for (auto& move : moves)
    {
        auto nextPosition = Position(position);
        nextPosition.play(move);

        int score = -negamax(nextPosition, depth, -infinity, infinity, colour);
        if (score > bestScore)
        {
            auto ptnMove = moveToPtn(move, position.size());
            mLogger << LogLevel::Debug << "New best move " << ptnMove << " with score " << score << Flush;
            bestScore = score;
            bestMove = &move;
        }
    }

    return moveToPtn(*bestMove, position.size());
}

int Engine::negamax(const Position& position, int depth, int alpha, int beta, int colour)
{
    if (depth == 0 || position.checkResult() != Result::None)
    {
        int score = evaluate(position);
        ++mStats.mNodeCount;

        // depth !=0 => game is over, we subtract depth here to favour quick wins and long losses
        return score * colour - depth;
    }

    auto moves = position.generateMoves();
    // auto orderedMoves = orderMoves(moves);

    int score = -infinity;
    for (auto& move : moves)
    {
        Position nextPosition(position);
        nextPosition.play(move);

        score = std::max(score, -negamax(nextPosition, depth - 1, beta * -1, alpha * -1, colour * -1));
        alpha = std::max(alpha, score);
        if (alpha >= beta)
           break; // Alpha Beta Cutoff Kapow!
    }

    return score;
}
