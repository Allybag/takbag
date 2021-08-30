#pragma once

#include "log/Logger.h"
#include "tak/Move.h"
#include "tak/Result.h"
#include "tak/Position.h"
#include "tak/RobinHoodHashes.h"
#include "../../external/robin_hood.h"
#include "TranspositionTable.h"
#include "OpeningBook.h"

#include <string>
#include <vector>

struct EngineStats
{
    std::size_t mNodeCount;
    EngineStats() : mNodeCount(0) { }
    void reset() { mNodeCount = 0; }
};

struct SearchResult
{
    Move mMove;
    int mScore;
    SearchResult(Move move, int score) : mMove(move), mScore(score) { }
    explicit SearchResult(int score) : SearchResult(Move(), score) { }
};

// We want to fix estimating next ply duration before we use a transposition table
template <bool UseTranspositionTable = true>
class Engine
{
    Logger mLogger{"Engine"};

    TranspositionTable mTranspositionTable;
    OpeningBook mOpeningBook;
    EngineStats mStats;
    std::vector<Move> mTopMoves;
    int64_t mStopSearchingTime{0};
    int mMaxDepth;

    Move chooseMoveFirst(const Position& position);
    Move deepeningSearch(const Position& position);

    int evaluatePos(const Position& position);
    int evaluateResult(Result result);
public:
    Engine() = default;
    explicit Engine(std::string openingBookFile) : mOpeningBook(openingBookFile) { }

    std::string chooseMove(const Position& position, double timeLimitSeconds = 3, int maxDepth = 15);
    Move chooseMoveRandom(const Position& position);
    void reset() { mTranspositionTable.clear(); }

    bool openingBookContains(const Position& position);
    int evaluate(const Position& position);
    SearchResult negamax(const Position &position, Move givenMove, int depth, int alpha, int beta, int colour);
};

#include <random>

template <typename Container>
auto chooseRandomElement(const Container& container) -> typename Container::const_iterator
{
    static std::random_device gRandomDevice;
    static std::default_random_engine gRandomEngine{ gRandomDevice() };
    std::uniform_int_distribution<std::size_t> range(0, container.size() - 1);
    auto element = container.begin();
    auto elementNum = range(gRandomEngine);
    std::advance(element, elementNum);
    return element;
}