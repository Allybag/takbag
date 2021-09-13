#include "TranspositionTable.h"

std::optional<TranspositionTableRecord> TranspositionTable::fetch(const Position& position, std::size_t depth) const
{
    auto hash = std::hash<Position>{}(position);
    auto record = (*mTable)[hash % sTableSize];

    // TODO: Using >= causes the engine to give up if it sees a loss further ahead in the table
    if (record.mHash == hash && record.mDepth == depth)
    {
        return record;
    }

    return std::nullopt;
}

void TranspositionTable::store(const Position& position, Move move, int score, uint8_t depth)
{
    auto hash = std::hash<Position>{}(position);
    auto& record = (*mTable)[hash % sTableSize];

    if (record.mHash == hash && record.mDepth >= depth)
        return;

    record = {hash, move, score, depth, ResultType::Unknown};
}

std::size_t TranspositionTable::count() const
{
    std::size_t entryCount = 0;
    for (const auto& entry : *mTable)
    {
        if (entry.mHash != 0)
            entryCount++;
    }

    return entryCount;
}
