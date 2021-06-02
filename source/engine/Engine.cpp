#include "Engine.h"
#include "MonteCarlo.h"
#include "tak/Position.h"
#include "other/Time.h"

#include <algorithm>
#include <optional>

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

std::string Engine::chooseMove(const Position& position, double timeLimitSeconds, int maxDepth)
{
    auto startTime = timeInMics();

    timeLimitSeconds = std::max(0.001, timeLimitSeconds); // Need at least a millisecond
    mStopSearchingTime = startTime + (timeLimitSeconds * micsInSecond);
    mMaxDepth = maxDepth;

    auto move = goodDeepeningSearch(position);

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
    MoveBuffer topMoves;
    while (timeInMics() < mStopSearchingTime)
    {
        ++depth;
        auto potentialMoves = chooseMovesNegamax(position, &move, depth);

        // If not we've almost certainly just aborted a search
        if (timeInMics() < mStopSearchingTime)
        {
            mLogger << LogLevel::Info << "Searched to depth " << depth << Flush;
            topMoves = potentialMoves;
            move = topMoves.front();
        }
    }

    // We use a 2 second Monte Carlo search as a tie breaker
    if (topMoves.size() > 1)
        move = monteCarloTreeSearch(position, 2, topMoves);

    return move;

}

Move Engine::goodDeepeningSearch(const Position& position)
{
    int depth = 0;
    Move move = Move();
    int colour = position.getPlayer() == Player::White ? 1 : -1;

    auto searchStart = timeInMics();
    auto lastSearchDuration = 0;
    mTopMoves.clear();
#if 0
    // TODO: Are either of these necessary or useful?
    mTranspositionTable.clear();
    mTranspositionTable.reserve(1024 * 1024 * 64);
#endif
    while (true)
    {
        ++depth;
        mTopMoves.emplace_back();

        auto searchResult = goodNegamax(position, move, depth, -infinity, infinity, colour);
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

Move Engine::chooseMoveNegamax(const Position& position, Move* potentialMove, int depth)
{
    return chooseMovesNegamax(position, potentialMove, depth).front();
}

MoveBuffer Engine::chooseMovesNegamax(const Position& position, Move* potentialMove, int depth)
{
    auto moves = position.generateMoves();

    int colour = position.getPlayer() == Player::Black ? 1 : -1;
    int bestScore = -infinity;

    const Move* bestMove = potentialMove;
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

    MoveBuffer topMoves;
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

std::optional<TranspositionTableRecord> lookup(const TranspositionTable& table, const Position& position, std::size_t depth)
{
    auto record = table.find(position);
    if (record != table.end() && record->second.mDepth >= depth)
    {
        return record->second;
    }

    return std::nullopt;
}

void store(TranspositionTable& table, const Position& position, std::size_t depth, int score)
{
    auto record = table.find(position);
    if (record == table.end() || record->second.mDepth < depth)
        table[position] = TranspositionTableRecord{score, depth};

}

SearchResult Engine::goodNegamax(const Position &position, Move givenMove, int depth, int alpha, int beta, int colour)
{
#if 0
    if (depth > 0)
    {
        auto record = lookup(mTranspositionTable, position, depth);
        if (record)
            return SearchResult(colour * record->mScore);
    }
#endif

    auto result = position.checkResult();
    if (result != Result::None)
    {
        ++mStats.mNodeCount;

        // We subtract depth here to favour quick wins and long losses
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

        auto score = goodNegamax(nextPosition, Move(), depth - 1, beta * -1, alpha * -1, colour * -1);
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

#if 0
    store(mTranspositionTable, position, depth, bestScore);
#endif
    return SearchResult(bestMove, bestScore);
}
