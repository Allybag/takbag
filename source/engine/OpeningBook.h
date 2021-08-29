#pragma once

#include <unordered_map>
#include <unordered_set>

#include "tak/Position.h"

using OpeningTable = std::unordered_map<Position, std::unordered_set<Move>>;

// We read a file containing openings
// We populate our hash table with all the next moves for a given position


class OpeningBook
{
    OpeningTable mOpeningTable;

public:
    explicit OpeningBook(const std::string& openingBookFile);
    bool contains(const Position& position) { return mOpeningTable.contains(position); }
    std::unordered_set<Move> getResponses(const Position& position) { return mOpeningTable[position]; }
    OpeningBook() = default;
};