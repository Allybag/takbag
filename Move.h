#pragma once

#include "Stone.h"

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
