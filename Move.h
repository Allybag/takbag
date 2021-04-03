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

struct Place
{
    std::size_t mIndex;
    Stone mStone;

    Place(std::size_t index, Stone stone) : mIndex(index), mStone(stone) { }
};

struct Move
{
    std::size_t mIndex;
    std::size_t mCount;

    // We have up to 8 numbers each at most 8, so we'll use a uint32_t to store eight quartets representing drop counts
    // 0 isn't a valid option, so we could use 8 trios where 000 represents 8, but uint24_t isn't really a thing anyway
    // TODO: Not really sure why I'm doing it this way...
    uint32_t mDropCounts;
    Direction mDirection;

    Move(std::size_t index, std::size_t count, uint32_t dropCounts, Direction direction) :
        mIndex(index), mCount(count), mDropCounts(dropCounts), mDirection(direction) { }
};
