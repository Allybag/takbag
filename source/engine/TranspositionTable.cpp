#include "TranspositionTable.h"

std::optional<TranspositionTableRecord> TranspositionTable::fetch(const Position& position, std::size_t depth) const
{
    auto record = mTable.find(position);
    // TODO: We can use records from deeper searches, but want to get some more tests up
    if (record != mTable.end() && record->second.mDepth == depth)
    {
        return record->second;
    }

    return std::nullopt;
}

void TranspositionTable::store(const Position& position, std::size_t depth, int score, Move move)
{
    // TODO: Very crude, fix me
#if 0
    constexpr std::size_t recordSize = sizeof(TranspositionTableRecord);
    if (mTable.size() * recordSize > 100'000'000)
    {
        mLogger << LogLevel::Info << "Clearing all " << mTable.size() <<  " entries!" << Flush;
        mLogger << LogLevel::Info << "Record size: " << recordSize << Flush;
        mTable.clear();
    }
#endif

    auto record = mTable.find(position);

    if (record != mTable.end() && record->second.mDepth >= depth)
        return;

    if (record != mTable.end())
        mTable.erase(record);

    mTable.try_emplace(position, position, depth, score, move);
}
