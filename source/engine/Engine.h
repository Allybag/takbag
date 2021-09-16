#pragma once

#include "../../external/robin_hood.h"
#include "OpeningBook.h"
#include "TranspositionTable.h"
#include "log/Logger.h"
#include "tak/Move.h"
#include "tak/Position.h"
#include "tak/Result.h"
#include "tak/RobinHoodHashes.h"

#include <string>
#include <vector>

struct EngineStats
{
    std::size_t mSeenNodes;      // How many times did we call negamax?
    std::size_t mEvaluatedNodes; // How many times did we call evaluate?
    std::size_t mTerminalNodes;  // How many times did we see positions where the game was over?
    std::size_t mTableHits;      // How many times did we see positions where the game was over?
    EngineStats() : mSeenNodes(0), mEvaluatedNodes(0), mTerminalNodes(0), mTableHits(0)
    {
    }
    void reset()
    {
        mEvaluatedNodes = mTerminalNodes = mSeenNodes = mTableHits = 0;
    }
};

inline std::ostream& operator<<(std::ostream& stream, EngineStats stats)
{
    stream << "Node counts: Seen(" << stats.mSeenNodes << "), Evaluated(" << stats.mEvaluatedNodes << "), Terminal("
           << stats.mTerminalNodes << "), TableHits(" << stats.mTableHits << ")";

    return stream;
}

// A valid evaluation function takes a position, returns a positive int if white is winning, negative if black
using EvaluationFunction = int (*)(const Position&);
extern EvaluationFunction gDefaultEvaluator;

struct EngineOptions
{
    bool mUseAlphaBeta;
    bool mUseMoveOrdering;
    bool mUseTranspositionTable;
    int mMaxDepth;
    std::string mOpeningBookPath;
    EvaluationFunction mEvaluator;

    EngineOptions(bool useAlphaBeta = true, bool useMoveOrdering = true, bool useTranspositionTable = false,
                  int maxDepth = 8, std::string openingBookPath = "", EvaluationFunction evaluator = gDefaultEvaluator)
        : mUseAlphaBeta(useAlphaBeta), mUseMoveOrdering(useMoveOrdering), mUseTranspositionTable(useTranspositionTable),
          mMaxDepth(maxDepth), mOpeningBookPath(openingBookPath), mEvaluator(evaluator)
    {
    }
};

struct EvaluationWeights
{
    int mFlatsOnBoardWeight;
    int mFlatCountWeight;
    int mCapsOnBoardWeight;  // 7s or 8s do have two caps
    int mStackControlWeight; // Probably want to make this much more fine grained
    int mStackControlNobleBonus;
    int mStoneOnEdgeWeight;
    int mIslandLengthsWeight; // For more info look in Position::countIslands in Position.cpp
};

extern EvaluationWeights gDefaultEvaluationWeights;

struct SearchResult
{
    Move mMove;
    int mScore;
    SearchResult(Move move, int score) : mMove(move), mScore(score)
    {
    }
    explicit SearchResult(int score) : SearchResult(Move(), score)
    {
    }
};

// We want to fix estimating next ply duration before we use a transposition table
// Plan: Engine takes an Evaluation function in its constructor
class Engine
{
    Logger mLogger{"Engine"};

    const bool mUseAlphaBeta = true;
    const bool mUseMoveOrdering = true;
    const bool mUseTranspositionTable = false;
    int mMaxDepth;

    const OpeningBook mOpeningBook;
    const EvaluationFunction mEvaluator;

    TranspositionTable mTranspositionTable{};
    EngineStats mStats;

    std::vector<Move> mTopMoves;
    int64_t mStopSearchingTime{0};

    Move chooseMoveFirst(const Position& position);
    Move deepeningSearch(const Position& position);

    int evaluateResult(Result result);

public:
    explicit Engine(EngineOptions options = EngineOptions())
        : mUseAlphaBeta(options.mUseAlphaBeta), mUseMoveOrdering(options.mUseMoveOrdering),
          mUseTranspositionTable(options.mUseTranspositionTable), mMaxDepth(options.mMaxDepth),
          mOpeningBook(options.mOpeningBookPath), mEvaluator(options.mEvaluator)
    {
    }

    std::string chooseMove(const Position& position, double timeLimitSeconds = 3, int maxDepth = 15);
    Move chooseMoveRandom(const Position& position);

    bool openingBookContains(const Position& position);
    int evaluate(const Position& position);
    SearchResult negamax(const Position& position, Move givenMove, int depth, int alpha, int beta, int colour);

    const EngineStats& getStats()
    {
        return mStats;
    }
};

#include <random>

template <typename Container> auto chooseRandomElement(const Container& container) -> typename Container::const_iterator
{
    static std::random_device gRandomDevice;
    static std::default_random_engine gRandomEngine{gRandomDevice()};
    std::uniform_int_distribution<std::size_t> range(0, container.size() - 1);
    auto element = container.begin();
    auto elementNum = range(gRandomEngine);
    std::advance(element, elementNum);
    return element;
}
