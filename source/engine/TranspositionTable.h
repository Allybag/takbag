#pragma once

#include "log/Logger.h"
#include "tak/Move.h"
#include "tak/Position.h"
#include "tak/RobinHoodHashes.h"

#include <optional>
#include <unordered_map>

enum ResultType : uint8_t
{
    Exact,
    LowerBound,
    UpperBound,
    Unknown
};

struct TranspositionTableRecord
{
    TranspositionTableRecord() : mHash(0), mMove(Move()), mScore(0), mDepth(0), mType(ResultType::Unknown)
    {
    }
    TranspositionTableRecord(uint64_t hash, const Move& move, int score, uint8_t depth, ResultType type)
        : mHash(hash), mMove(move), mScore(score), mDepth(depth), mType(type)
    {
    }

    uint64_t mHash;
    Move mMove;
    int mScore;
    uint8_t mDepth;
    ResultType mType;
};

class TranspositionTable
{
    Logger mLogger{"Engine"};
    static constexpr std::size_t sTableSize = 1 << 22; // 4 million entries * 24 bytes = 64 Megs

    using TableT = std::array<TranspositionTableRecord, sTableSize>;
    TableT* mTable;

public:
    TranspositionTable()
    {
        mTable = new TableT{};
    }

    ~TranspositionTable()
    {
        delete mTable;
    }

public:
    std::optional<TranspositionTableRecord> fetch(const Position& position, std::size_t depth) const;
    void store(const Position& position, Move move, int score, uint8_t depth, ResultType type);

    std::size_t count() const;
};
