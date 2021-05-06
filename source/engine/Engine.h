#pragma once

#include "log/Logger.h"
#include "tak/Move.h"
#include "tak/Result.h"

#include <string>
#include <vector>

class Position;

struct EngineStats
{
    std::size_t mNodeCount;
    EngineStats() : mNodeCount(0) { }
    void reset() { mNodeCount = 0; }
};

class Engine
{
    EngineStats mStats;
    Logger mLogger{"Engine"};
    int64_t mStopSearchingTime{0};

    Move chooseMoveFirst(const Position& position);
    Move chooseMoveRandom(const Position& position);
    Move deepeningSearch(const Position& position);

    int evaluatePos(const Position& position);
    int evaluateResult(Result result);
public:
    std::string chooseMove(const Position& position, int timeLimitSeconds = 3);
    Move chooseMoveNegamax(const Position& position, Move* move, int depth);
    std::vector<Move> chooseMovesNegamax(const Position& position, Move* move, int depth); // Returns all best moves

    int evaluate(const Position& position);
    int negamax(const Position &position, int depth, int alpha, int beta, int colour);
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

