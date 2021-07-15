#include "TranspositionTable.h"

std::optional<TranspositionTableRecord> TranspositionTable::fetch(const Position& position, std::size_t depth) const
{
    auto record = mTable.find(position);
    if (record != mTable.end() && record->second.mDepth >= depth)
    {
        return record->second;
    }

    return std::nullopt;
}

void TranspositionTable::store(const Position& position, std::size_t depth, int score, Move move)
{
    auto record = mTable.find(position);

    if (record != mTable.end() && record->second.mDepth >= depth)
        return;

    if (record != mTable.end())
        mTable.erase(record);

    mTable.try_emplace(position, depth, score, move);
}

void TranspositionTable::clear()
{
    constexpr std::size_t recordSize = sizeof(TranspositionTableRecord);
    if (mTable.size())
    {
        mLogger << LogLevel::Info << "Transposition Table size: " << mTable.size() * recordSize << " bytes" << Flush;
        mLogger << LogLevel::Info << "Clearing all " << mTable.size() <<  " entries!" << Flush;
        mTable.clear();
    }

}