#pragma once

#include "log/Logger.h"
#include "tak/Move.h"
#include "tak/Position.h"
#include "tak/RobinHoodHashes.h"

#include <unordered_map>
#include <optional>

struct TranspositionTableRecord
{
    std::size_t mDepth;
    int mScore;
    Move mMove;

    TranspositionTableRecord(std::size_t depth, int score, Move move) : mDepth(depth), mScore(score), mMove(move) { };
};

class TranspositionTable
{
    Logger mLogger{"Engine"};

    using TableT = robin_hood::unordered_flat_map<Position, TranspositionTableRecord, Hash<Position>>;
    TableT mTable;

public:
    std::optional<TranspositionTableRecord> fetch(const Position& position, std::size_t depth) const;
    void store(const Position& position, std::size_t depth, int score, Move move);
    void clear();
};