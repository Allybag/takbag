#pragma once

#include "Direction.h"
#include "Stone.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

enum class MoveType
{
    Place, // (stone)(square)
    Move   // (count)(square)(direction)(drop counts)(stone)
};

struct Move
{
    // Common to all moves
    Direction mDirection{Direction::None};
    uint8_t mIndex{0};

    // Only for MoveType::Place moves
    StoneType mStoneType{StoneType::Blank};

    // Only for MoveType::Move moves
    uint8_t mCount{0};

    // We have up to 8 numbers each at most 8, so we'll use a uint32_t to store eight quartets representing drop counts
    // 0 isn't a valid option, so we could use 8 trios where 000 represents 8, but uint24_t isn't really a thing anyway
    uint32_t mDropCounts{0};

    Move() = default;
    Move(std::size_t index, StoneType stoneType) : mDirection(Direction::None), mIndex(index), mStoneType(stoneType)
    {
    }
    Move(std::size_t index, std::size_t count, uint32_t dropCounts, Direction direction)
        : mDirection(direction), mIndex(index), mCount(count), mDropCounts(dropCounts)
    {
    }

    template <typename LambdaT> // Lambda should take a single uint8_t
    void forEachStone(LambdaT lambda) const;
};

static_assert(std::is_trivially_copyable_v<Move>);
static_assert(sizeof(Move) == 8); // That's nice

// Not using MoveBuffer.h unless good benchmarks show it to be quicker
#include <vector>
using MoveBuffer = std::vector<Move>;

inline bool operator==(const Move& lhs, const Move& rhs)
{
    return std::tie(lhs.mIndex, lhs.mStoneType, lhs.mCount, lhs.mDropCounts, lhs.mDirection) ==
           std::tie(rhs.mIndex, rhs.mStoneType, rhs.mCount, rhs.mDropCounts, rhs.mDirection);
}

inline bool operator!=(const Move& lhs, const Move& rhs)
{
    return !(lhs == rhs);
}

inline bool isSet(const Move& move)
{
    return (move.mDirection != Direction::None || move.mStoneType != StoneType::Blank);
}

inline std::ostream& operator<<(std::ostream& stream, const Move& move)
{
    if (move.mDirection == Direction::None)
    {
        stream << "Place a " << move.mStoneType << " at index " << move.mIndex;
    }
    else
    {
        stream << "Move " << move.mCount << " stones from index " << move.mIndex << " " << move.mDirection;
        stream << " dropping ";

        auto printStone = [&stream](uint8_t dropCount) { stream << static_cast<int>(dropCount) << " "; };

        move.forEachStone(printStone);
        stream << "stones";
    }
    return stream;
}

template <typename LambdaT> inline void Move::forEachStone(LambdaT lambda) const
{
    uint32_t dropCountMask = 0xf; // Last four bits set
    uint8_t stonesLeftToDrop = mCount;
    for (uint8_t i = 0; stonesLeftToDrop != 0; ++i)
    {
        uint8_t dropCount = (mDropCounts & (dropCountMask << i * 4)) >> (i * 4);
        stonesLeftToDrop -= dropCount;
        assert(dropCount > 0);
        assert(dropCount <= 0x8);
        lambda(dropCount);
    }
}

inline std::size_t axisToIndex(uint8_t col, uint8_t rank, std::size_t size)
{
    return col + (rank * size);
}

inline std::pair<uint8_t, uint8_t> indexToAxis(uint8_t index, std::size_t size)
{
    uint8_t col = index % size;
    uint8_t rank = index / size;
    return std::make_pair(col, rank);
}

inline std::string moveToPtn(const Move& move, std::size_t size)
{
    std::string ptn;
    auto [col, rank] = indexToAxis(move.mIndex, size);

    if (move.mDirection == Direction::None)
    {
        if (isCap(move.mStoneType))
            ptn.push_back('C');
        else if (isWall(move.mStoneType))
            ptn.push_back('S');

        ptn.push_back('a' + col);
        ptn.push_back('1' + rank);
    }
    else
    {
        ptn.push_back('0' + move.mCount);
        ptn.push_back('a' + col);
        ptn.push_back('1' + rank);
        ptn.push_back(static_cast<char>(move.mDirection));

        auto addCountToPtn = [&ptn](uint8_t count) { ptn.push_back('0' + count); };
        move.forEachStone(addCountToPtn);
    }

    return ptn;
}

namespace std
{
template <> struct hash<Move>
{
    std::size_t operator()(const Move& move) const
    {
        std::size_t directionHash = std::hash<Direction>{}(move.mDirection);
        std::size_t indexHash = std::hash<uint8_t>{}(move.mIndex);
        std::size_t stoneHash = std::hash<StoneType>{}(move.mStoneType);
        std::size_t countHash = std::hash<uint8_t>{}(move.mCount);
        std::size_t dropsHash = std::hash<uint32_t>{}(move.mDropCounts);

        return directionHash ^ (indexHash << 1) ^ (stoneHash << 2) ^ (countHash << 3) ^ (dropsHash << 4);
    }
};
} // namespace std
