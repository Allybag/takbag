#pragma once

#include "Stone.h"

#include <string>
#include <sstream>

// Representation:
// mTopStone describes the surface stone and mStack[0]
// When we add a stone of colour c, we set mStack[mCount] = c, increment mCount and set mTopStone accordingly
// When we remove n stones, we subtract n from mCount, and set mTopStone = mStack[mCount - 1]
struct Square
{
    Stone mTopStone;
    uint8_t mCount;
    uint32_t mStack; // A mCount long bitSet, 0 is white and 1 is black

    Square() : mTopStone(Stone::Blank), mCount(0), mStack(0) { }
    Square(const Square&) noexcept = default;
    Square(Square&&) noexcept = default;
    Square& operator=(const Square&) noexcept = default;
    Square& operator=(Square&&) noexcept = default;
    ~Square() = default;

    std::size_t count() const;
    std::string print() const;

    void give(Stone stone, uint8_t count, uint8_t allStones);
    void take(std::size_t count);
};

std::size_t Square::count() const
{
    return mCount;
}

std::string Square::print() const
{
    std::stringstream output;
    switch (mTopStone)
    {
        case Stone::Blank:
            return " ";
        case Stone::WhiteFlat:
            output << "F";
            break;
        case Stone::BlackFlat:
            output << "f";
            break;
        case Stone::WhiteWall:
            output << "S";
            break;
        case Stone::BlackWall:
            output << "s";
            break;
        case Stone::WhiteCap:
            output << "C";
            break;
        case Stone::BlackCap:
            output << "c";
            break;
    }

    // TODO: Test this bit twiddling
    for (std::size_t i = 1; i < mCount; ++i) // Start at 1 as we've already done mTopStone
        output << (mStack & (1 << i) ? "f" : "F");

    return output.str();
}

void Square::give(Stone topStone, uint8_t count, uint8_t allStones)
{
    assert(!(mTopStone & StoneBits::Standing));
    assert(count);
    assert(mCount + count < 32); // Could have more than 32 stones in a stack

    mTopStone = topStone;
    for (std::size_t i = 0; i < count; ++i)
    {
        int currentStone = allStones & (1 << i);
        // Apparently works for 2's complement representation, now guaranteed in c++
        // Sets the mCount bit of mStack to currentStone
        // TODO: Test this bit twiddling
        mStack ^= (-currentStone ^ mStack) & (1 << mCount);
        mCount++;
    }
}

void Square::take(std::size_t count)
{
    assert(mTopStone != Stone::Blank);
    assert(mCount >= count);

    // We let mStack contain old garbage beyone mCount as we shouldn't use it
    mCount -= count;
    mTopStone = (mCount == 0 ? Stone::Blank : (mStack & 1 << mCount) ? Stone::BlackFlat : Stone::WhiteFlat);
}
