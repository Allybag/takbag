#include "Engine.h"
#include "tak/Position.h"
#include "other/Time.h"

#include <algorithm>
#include <optional>

static constexpr int winValue = 1000;
static constexpr int infinity = 100001; // Not really infinity, but pretty high

EvaluationWeights gDefaultEvaluationWeights = EvaluationWeights{8, 12, 9, 1, 1, 1, 4};

int Engine::evaluatePos(const Position& position)
{
    int score = 0;

    auto reserveCounts = position.getReserveCount();
    score -= reserveCounts[Player::White] * mWeights.mFlatsOnBoardWeight;
    score += reserveCounts[Player::Black] * mWeights.mFlatsOnBoardWeight;

    auto size = position.size();
    for (std::size_t index = 0; index < size * size; ++index)
    {
        auto square = position[index];
        auto topStone = square.mTopStone;
        if (square.mTopStone == Stone::Blank)
            continue;

        auto colour = topStone & StoneBits::Black ? -1 : 1;

        if (isFlat(square.mTopStone))
            score += mWeights.mFlatCountWeight * colour; // We want a high flat count

        if (isCap(square.mTopStone))
            score += mWeights.mCapsOnBoardWeight * colour; // We want to use our cap rather than walls if possible

        score += square.mCount * mWeights.mStackControlWeight * colour; // Wanna control stacks
        if (topStone & StoneBits::Standing)
            score += square.mCount * mWeights.mStackControlNobleBonus * colour; // Wanna control stacks especially with

        if ((index < size) || (index >= (size * size) - size) || (index % size == 0) || (index % size == size - 1))
            score -= mWeights.mStoneOnEdgeWeight * colour; // Lose a point for a square on the edge
    }

    // This is just a constant offset to all scores, and so completely pointless. Still..
    score -= static_cast<int>(mWeights.mFlatCountWeight * position.getKomi()); // A positive komi is points for black

    // TODO: check if this is too slow to be worth it
    auto islandLengths = position.countIslands();
    score += islandLengths[Player::White] * mWeights.mIslandLengthsWeight; // We want our pieces as connected as possible
    score -= islandLengths[Player::Black] * mWeights.mIslandLengthsWeight;

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

// Returns a score from white's point of view
int Engine::evaluate(const Position& position)
{
    auto result = position.checkResult();
    if (result != Result::None)
        return evaluateResult(result);

    return evaluatePos(position);
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

SearchResult Engine::negamax(const Position &position, Move givenMove, int depth, int alpha, int beta, int colour)
{
    if constexpr(UseTranspositionTable)
    {
        auto record = mTranspositionTable.fetch(position, depth);
        if (record)
        {
            return {record->mMove, record->mScore};
        }
    }

    auto result = position.checkResult();
    if (result != Result::None)
    {
        ++mStats.mNodeCount;

        int score = evaluateResult(result);
        return SearchResult(colour * score * (depth + 1));
    }

    if (depth == 0)
    {
        ++mStats.mNodeCount;

        int score = evaluatePos(position);
        return SearchResult(colour * score);
    }


    Move bestMove = Move();
    int bestScore = -infinity;
    auto moves = position.generateMoves();
    auto topMoveIndex = mTopMoves.size() - depth;
    auto topMove = mTopMoves[topMoveIndex];

    if (isSet(givenMove) || isSet(topMove))
    {
        for (auto it = moves.begin(); it != moves.end(); ++it)
        {
            if (topMove == *it)
            {
                mLogger << LogLevel::Debug << "Swapping top move " << moveToPtn(givenMove, position.size()) << " to front" << Flush;
                std::iter_swap(it, moves.begin() + 1);
                break;
            }

            if (givenMove == *it)
            {
                mLogger << LogLevel::Debug << "Swapping given move " << moveToPtn(givenMove, position.size()) << " to front" << Flush;
                std::iter_swap(it, moves.begin());
                break;
            }
        }
    }

    for (auto &move : moves)
    {
        Position nextPosition(position);
        nextPosition.play(move);

        auto score = negamax(nextPosition, Move(), depth - 1, beta * -1, alpha * -1, colour * -1);
        score.mScore *= -1;

        if (score.mScore > bestScore)
        {
            bestScore = score.mScore;
            bestMove = move;
            mTopMoves[topMoveIndex] = move;
        }

        alpha = std::max(alpha, score.mScore);
        if (alpha >= beta)
            break; // Alpha Beta Cutoff Kapow!
    }

    if constexpr(UseTranspositionTable)
        mTranspositionTable.store(position, bestMove, bestScore, depth);

    return {bestMove, bestScore};
}

Move Engine::deepeningSearch(const Position& position)
{
    int depth = 0;
    Move move = Move();
    int colour = position.getPlayer() == Player::White ? 1 : -1;

    auto searchStart = timeInMics();
    auto lastSearchDuration = 0;
    mTopMoves.clear();
    while (true)
    {
        ++depth;
        mTopMoves.emplace_back();

        auto searchResult = negamax(position, move, depth, -infinity, infinity, colour);
        auto searchStop = timeInMics();

        move = searchResult.mMove;
        mLogger << LogLevel::Info << "Best move " << moveToPtn(move, position.size())
                << " with score " << searchResult.mScore  << " at depth " << depth << Flush;

        auto searchDuration = searchStop - searchStart;
        auto searchIncreaseFactor = lastSearchDuration ? searchDuration / lastSearchDuration : 1;
        lastSearchDuration = searchDuration;
        searchStart = searchStop;
        mLogger << LogLevel::Info << "Search took " << searchDuration << " mics" << Flush;
        mLogger << LogLevel::Info << "Estimated next search duration: " << searchDuration * searchIncreaseFactor << Flush;

        if (searchStop + searchDuration * searchIncreaseFactor > mStopSearchingTime)
            break;

        // If we've already found a loss, then might as well stop searching
        if (std::abs(searchResult.mScore) >= winValue)
        {
            mStopSearchingTime = timeInMics();
            mLogger << LogLevel::Info << "Stopping search after finding end of game at depth " << depth << Flush;
            break;
        }

        if (depth >= mMaxDepth)
        {
            break;
        }
    }

    return move;

}

bool Engine::openingBookContains(const Position &position)
{
    Shift canonicalShift = position.getCanonicalShift();
    Position canonicalPosition = position.shift(canonicalShift);
    return mOpeningBook.contains(canonicalPosition);
}

std::string Engine::chooseMove(const Position& position, double timeLimitSeconds, int maxDepth)
{
    auto startTime = timeInMics();

    Shift canonicalShift = position.getCanonicalShift();
    Position canonicalPosition = position.shift(canonicalShift);
    mLogger << LogLevel::Info << "Shift: " << canonicalShift << Flush;
    if (mOpeningBook.contains(canonicalPosition))
    {
        auto openingMove = *chooseRandomElement(mOpeningBook.getResponses(canonicalPosition));
        openingMove.mIndex = applyShift(openingMove.mIndex, canonicalPosition.size(), getReverseShift(canonicalShift));
        mLogger << LogLevel::Info << "Playing move from opening book" << Flush;
        return moveToPtn(openingMove, canonicalPosition.size());
    }

    timeLimitSeconds = std::max(0.001, timeLimitSeconds); // Need at least a millisecond
    mStopSearchingTime = startTime + static_cast<int64_t>(timeLimitSeconds * micsInSecond);
    mMaxDepth = maxDepth;

    auto move = deepeningSearch(position);

    auto stopTime = timeInMics();
    auto duration = stopTime - startTime;
    mLogger << LogLevel::Info << "Evaluated " << mStats.mNodeCount << " nodes in " << duration << " mics" << Flush;

    mStats.reset();
    mStopSearchingTime = 0;
    return moveToPtn(move, position.size());
}

