#include "Engine.h"
#include "tak/Position.h"
#include "MonteCarlo.h"

#include <algorithm>
#include "other/Time.h"

static constexpr int winValue = 1000;
static constexpr int infinity = 100001; // Not really infinity, but pretty high

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
    int score = 0;

    auto reserveCounts = position.getReserveCount();
    score -= reserveCounts[Player::White] * 8; // We want flats on the board
    score += reserveCounts[Player::Black] * 8;

    auto size = position.size();
    for (std::size_t index = 0; index < size * size; ++index)
    {
        auto square = position[index];
        auto topStone = square.mTopStone;
        if (square.mTopStone == Stone::Blank)
            continue;

        auto colour = topStone & StoneBits::Black ? -1 : 1;

        if (isFlat(square.mTopStone))
            score += 12 * colour; // We want a high flat count

        if (isCap(square.mTopStone))
            score += 9 * colour; // We want to use our cap rather than walls if possible

        score += square.mCount * colour; // Wanna control stacks
        if (topStone & StoneBits::Standing)
            score += square.mCount * colour; // Wanna control stacks especially with

        if ((index < size) || (index >= (size * size) - size) || (index % size == 0) || (index % size == size - 1))
            score -= colour; // Lose a point for a square on the edge
    }

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

std::string Engine::chooseMove(const Position& position, int timeLimitSeconds)
{
    auto startTime = timeInMics();

    timeLimitSeconds = std::max(1, timeLimitSeconds); // Need at least a second
    mStopSearchingTime = startTime + (timeLimitSeconds * micsInSecond);

    auto move = deepeningSearch(position);

    auto stopTime = timeInMics();
    auto duration = stopTime - startTime;
    mLogger << LogLevel::Info << "Evaluated " << mStats.mNodeCount << " nodes in " << duration << " mics" << Flush;

    mStats.reset();
    mStopSearchingTime = 0;
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

Move Engine::deepeningSearch(const Position& position)
{
    int depth = 0;
    Move move = chooseMoveNegamax(position, nullptr, 0);
    std::vector<Move> topMoves;
    while (timeInMics() < mStopSearchingTime)
    {
        ++depth;
        auto potentialMoves = chooseMovesNegamax(position, &move, depth);
        move = potentialMoves.front();

        // If not we've almost certainly just aborted a search
        if (timeInMics() < mStopSearchingTime)
        {
            mLogger << LogLevel::Info << "Searched to depth " << depth << Flush;
            topMoves = potentialMoves;
        }
    }

    // We use a 2 second Monte Carlo search as a tie breaker
    if (topMoves.size() > 1)
        move = monteCarloTreeSearch(position, 2, topMoves);

    return move;

}

Move Engine::chooseMoveNegamax(const Position& position, Move* potentialMove, int depth)
{
    return chooseMovesNegamax(position, potentialMove, depth).front();
}

std::vector<Move> Engine::chooseMovesNegamax(const Position& position, Move* potentialMove, int depth)
{
    auto moves = position.generateMoves();

    int colour = position.getPlayer() == Player::Black ? 1 : -1;
    int bestScore = -infinity;

    Move* bestMove = potentialMove;
    // TODO: I realised the below move ordering is completely pointless
    // Literally gains nothing, as we do a distinct alpha beta search per starting move
    // So even if we start with the best move, we pass no information along to the next search
#if 0
    if (bestMove != nullptr)
    {
        for (auto it = moves.begin(); it != moves.end(); ++it)
            if (*bestMove == *it)
            {
                std::iter_swap(it, moves.begin());
                break;
            }
    }
#endif

    std::vector<Move> topMoves;
    for (auto& move : moves)
    {
        if (mStopSearchingTime != 0)
        {
            auto now = timeInMics();
            if (now > mStopSearchingTime)
            {
                if (potentialMove == nullptr)
                    mLogger << LogLevel::Warn << "Hit time limit but no move to return!" << Flush;
                else
                {
                    mLogger << LogLevel::Info << "Aborting search " << Flush;
                    return topMoves;
                }
            }
        }

        auto nextPosition = Position(position);
        nextPosition.play(move);

        int score = -negamax(nextPosition, depth, -infinity, infinity, colour);
        if (score > bestScore)
        {
            auto ptnMove = moveToPtn(move, position.size());
            mLogger << LogLevel::Debug << "New best move " << ptnMove << " with score " << score << Flush;
            bestScore = score;
            bestMove = &move;
            topMoves.clear();
            topMoves.push_back(move);
        }
        else if (score == bestScore)
        {
            topMoves.push_back(move);
        }
    }

    auto ptnMove = moveToPtn(*bestMove, position.size());
    mLogger << LogLevel::Info << topMoves.size() << " Best moves including " << ptnMove << " with score " << bestScore << Flush;

    if (std::abs(bestScore) >= winValue && mStopSearchingTime != 0)
    {
        mStopSearchingTime = timeInMics();
        mLogger << LogLevel::Info << "Stopping search after finding end of game" << Flush;
    }

    return topMoves;
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
