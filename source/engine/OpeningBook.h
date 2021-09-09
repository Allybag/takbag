#pragma once

#include <unordered_map>
#include <vector>

#include "tak/Position.h"
#include "log/Logger.h"

using OpeningTable = std::unordered_map<Position, MoveBuffer>;

// We read a file containing openings
// We populate our hash table with all the next moves for a given position


class OpeningBook
{
    Logger mLogger{"Openings"};
    OpeningTable mOpeningTable;

public:
    explicit OpeningBook(const std::string& openingBookFile);
    bool contains(const Position& position) { return mOpeningTable.contains(position); }
    MoveBuffer getResponses(const Position& position) { return mOpeningTable[position]; }
    OpeningBook() = default;
};
