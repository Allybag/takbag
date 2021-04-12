#pragma once

#include "Stone.h"
#include <iostream>
#include <string>
#include <utility>
#include <cstdint>

enum class Direction : uint8_t
{
    Up = '+',
    Down = '-',
    Left = '<',
    Right = '>',
    None = 0
};

const Direction Directions[] = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};

enum class MoveType
{
    Place, // (stone)(square)
    Move // (count)(square)(direction)(drop counts)(stone)
};

struct Move
{
    // Common to all moves
    MoveType mType;
    std::size_t mIndex;

    // Only for MoveType::Place moves
    Stone mStone{Stone::Blank};

    // Only for MoveType::Move moves
    std::size_t mCount{0};

    // We have up to 8 numbers each at most 8, so we'll use a uint32_t to store eight quartets representing drop counts
    // 0 isn't a valid option, so we could use 8 trios where 000 represents 8, but uint24_t isn't really a thing anyway
    // TODO: Not really sure why I'm doing it this way...
    uint32_t mDropCounts{0};
    Direction mDirection{Direction::None};

    Move(std::size_t index, Stone stone) : mType(MoveType::Place), mIndex(index), mStone(stone) { }
    Move(std::size_t index, std::size_t count, uint32_t dropCounts, Direction direction) :
        mType(MoveType::Move), mIndex(index), mCount(count), mDropCounts(dropCounts), mDirection(direction) { }

    template <typename LambdaT> // Lambda should take a single uint8_t
    void forEachStone(LambdaT lambda) const;
};

inline bool operator==(const Move& lhs, const Move& rhs)
{
    return std::tie(lhs.mType, lhs.mIndex, lhs.mStone, lhs.mCount, lhs.mDropCounts, lhs.mDirection) ==
           std::tie(rhs.mType, rhs.mIndex, rhs.mStone, rhs.mCount, rhs.mDropCounts, rhs.mDirection);
}

inline bool operator!=(const Move& lhs, const Move& rhs)
{
    return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& stream, const Direction& dir)
{
    switch (dir)
    {
        case Direction::Up:
            stream << "Up";
            break;
        case Direction::Down:
            stream << "Down";
            break;
        case Direction::Left:
            stream << "Left";
            break;
        case Direction::Right:
            stream << "Right";
            break;
        case Direction::None:
            assert(false);
            break;
    }
    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const Move& move)
{
    if (move.mType == MoveType::Place)
    {
        stream << "Place a " << move.mStone << " at index " << move.mIndex;
    }
    else
    {
        stream << "Move " << move.mCount << " stones from index " << move.mIndex << " " << move.mDirection;
        stream << " dropping ";

        auto printStone = [&stream](uint8_t dropCount)
        {
            stream << static_cast<int>(dropCount) << " ";
        };

        move.forEachStone(printStone);
        stream << "stones";
    }
    return stream;
}

template <typename LambdaT>
inline void Move::forEachStone(LambdaT lambda) const
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

    if (move.mType == MoveType::Place)
    {
        if (isCap(move.mStone))
            ptn.push_back('C');
        else if (isWall(move.mStone))
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

        auto addCountToPtn = [&ptn](uint8_t count)
        {
            ptn.push_back('0' + count);
        };
        move.forEachStone(addCountToPtn);
    }

    return ptn;
}
