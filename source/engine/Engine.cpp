#include "Engine.h"
#include "tak/Position.h"
#include "MonteCarlo.h"

#include <chrono>
#include <algorithm>

static constexpr int winValue = 100;
static constexpr int infinity = 10001; // Not really infinity, but pretty high

// Returns a score from white's point of view
int Engine::evaluate(const Position& position)
{
    auto result = position.checkResult();
    if (result != Result::None)
        return evaluateResult(result);

    return evaluatePos(position);
}

int Engine::evaluatePos(const Position& position)
{
    auto flatCounts = position.checkFlatCount();
    int score = 0;
    score += static_cast<int>(flatCounts[Player::White]) * 2; // A top flat on the board is worth two points
    score -= static_cast<int>(flatCounts[Player::Black]) * 2;

    auto reserveCounts = position.getReserveCount();
    score -= reserveCounts[Player::White]; // A flat on the board is worth one point
    score += reserveCounts[Player::Black];

    return score;
}

int Engine::evaluateResult(Result result)
{
    assert(result != Result::None);
    if (result == Result::Draw)
        return 0;

    if (result & StoneBits::Black)
        return -winValue;

    return winValue;
}

std::string Engine::chooseMove(const Position& position)
{
    using namespace std::chrono;
    auto startTime = steady_clock::now();
    mStats.reset();

    auto move = deepeningSearch(position);

    auto stopTime = steady_clock::now();
    auto duration = duration_cast<microseconds>(stopTime - startTime);
    mLogger << LogLevel::Info << "Evaluated " << mStats.mNodeCount << " nodes in " << duration.count() << " mics" << Flush;
    return moveToPtn(move, position.size());
}

Move Engine::chooseMoveFirst(const Position& position)
{
    assert(position.checkResult() == Result::None);
    auto moves = position.generateMoves();
    auto move = moves.front();
    return move;
}

Move Engine::chooseMoveRandom(const Position& position)
{
    assert(position.checkResult() == Result::None);
    auto moves = position.generateMoves();
    auto randomMove = chooseRandomElement(moves);
    return *randomMove;
}

Move Engine::deepeningSearch(const Position& position, int maxSeconds)
{
    using namespace std::chrono;
    auto endTime = steady_clock::now() + std::chrono::seconds(maxSeconds);
    int depth = 0;
    Move move = chooseMoveNegamax(position, nullptr, 0);
    while (steady_clock::now() < endTime)
    {
        ++depth;
        move = chooseMoveNegamax(position, &move, depth);
        mLogger << LogLevel::Info << "Searched to depth  " << depth << Flush;
    }

    return move;

}

Move Engine::chooseMoveNegamax(const Position& position, Move* potentialMove, int depth)
{
    auto moves = position.generateMoves();

    int colour = position.getPlayer() == Player::Black ? 1 : -1;
    int bestScore = -infinity;

    Move* bestMove = potentialMove;
    if (bestMove != nullptr)
    {
        for (auto it = moves.begin(); it != moves.end(); ++it)
            if (*bestMove == *it)
            {
                std::iter_swap(it, moves.begin());
                break;
            }
    }

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

    return *bestMove;
}

int Engine::negamax(const Position& position, int depth, int alpha, int beta, int colour)
{
    auto result = position.checkResult();
    if (result != Result::None)
    {
        ++mStats.mNodeCount;

        // We subtract depth here to favour quick wins and long losses
        int score = evaluateResult(result);
        return colour * score * (depth + 1);
    }

    if (depth == 0)
    {
        ++mStats.mNodeCount;

        int score = evaluatePos(position);
        return colour * score;
    }

    auto moves = position.generateMoves();

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
