#pragma once

#include "log/Logger.h"
#include "tak/Move.h"
#include "tak/Result.h"
#include "tak/Position.h"

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

class Engine
{
    Logger mLogger{"Engine"};

    EngineStats mStats;
    std::vector<Move> mTopMoves;
    int64_t mStopSearchingTime{0};
    int mMaxDepth;

    Move chooseMoveFirst(const Position& position);
    Move chooseMoveRandom(const Position& position);
    Move deepeningSearch(const Position& position);
    Move goodDeepeningSearch(const Position& position);

    int evaluatePos(const Position& position);
    int evaluateResult(Result result);
public:
    std::string chooseMove(const Position& position, double timeLimitSeconds = 3, int maxDepth = 15);
    Move chooseMoveNegamax(const Position& position, Move* move, int depth);
    const MoveBuffer chooseMovesNegamax(const Position& position, Move* move, int depth); // Returns all best moves

    int evaluate(const Position& position);
    int negamax(const Position &position, int depth, int alpha, int beta, int colour);
    SearchResult goodNegamax(const Position &position, Move givenMove, int depth, int alpha, int beta, int colour);
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

