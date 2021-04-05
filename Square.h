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
    Square(Stone topStone, uint8_t count, uint32_t stack) : mTopStone(topStone), mCount(count), mStack(stack) { }
    Square(const Square&) noexcept = default;
    Square(Square&&) noexcept = default;
    Square& operator=(const Square&) noexcept = default;
    Square& operator=(Square&&) noexcept = default;
    ~Square() = default;

    // Take count pieces from this square to make another
    // This constructor is basically a "take stones" function
    Square(Square& source, uint8_t count); // Note, mutates source
    void add(Square& source, uint8_t count);

    std::size_t count() const;
    std::string print() const;

    // Utility functions
    void clearBeyondStack();
    void setTopFromStack();
    void checkValid();

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
    for (std::size_t i = mCount - 1; i != 0; --i) // We start at mCount - 1 as we have already printed the top stone
        output << (mStack & (1 << (i - 1)) ? "f" : "F");

    if (mCount > 1)
        std::cerr << "Top: " << this->mTopStone << " count: " << static_cast<int>(this->mCount) << " stack: " << this->mStack << std::endl;

    return output.str();
}

void Square::add(Square& source, uint8_t count)
{
    assert(!isCap(mTopStone));
    assert(count);
    assert(source.mCount >= count);
    assert(mCount + count < 32); // Could have more than 32 stones in a stack

    // Wall smashes
    if (isWall(mTopStone))
    {
        assert(source.mCount == 1 && isCap(source.mTopStone));
        std::cerr << "Wall smash!" << std::endl;
        // We don't actually use mTopStone again, so we don't need to flatten it
    }

    // TODO: Test all this!
    Square oldSource(source);
    Square oldThis(*this);

    int stonesLeftInSource = source.mCount - count;
    // We want just the bits in source.mStack from source.mCount to source.mCount - count
    // We want a mask which has all bits set to 1, except for the stonesLeftInSource bits on the left
    // And then we shift that to the beginning
    uint32_t allStonesMask = static_cast<uint32_t>(-1);
    uint32_t hideLowStonesMask = (1 << stonesLeftInSource) - 1;
    uint32_t mask = allStonesMask - hideLowStonesMask;

    uint32_t movingStones = (source.mStack & mask); // Just the stones we need to move
    movingStones = movingStones >> stonesLeftInSource;
    movingStones = movingStones << mCount;

    // Deal with this square
    mCount += count;
    mStack += movingStones;

    // Deal with the old square
    // We drop from the bottom
    source.mCount -= count;
    source.mStack >>= count;
    // source.clearBeyondStack(); // Should be unnecessary?

    // Deal with the topStones
    if (source.mCount)
    {
        setTopFromStack();
    }
    else
    {
        mTopStone = source.mTopStone;
        source.setTopFromStack(); // Or just source.mTopStone = Stone::Blank
        assert(source.mTopStone == Stone::Blank);
    }

    checkValid();
    source.checkValid();
}

Square::Square(Square& source, uint8_t count)
{
    assert(source.mTopStone != Stone::Blank);
    assert(source.mCount >= count);

    // Initialise our members
    mTopStone = source.mTopStone;
    mCount = count;
    mStack = (source.mStack >> (source.mCount - count)); // TODO: Test this!

    // Remove pieces from the old stone
    source.mCount -= count;
    source.clearBeyondStack();
    source.setTopFromStack();

    checkValid();
    source.checkValid();
}

void Square::clearBeyondStack()
{
    // TODO: Test this
    uint32_t mask = (1 << mCount) - 1;
    mStack &= mask;
}

void Square::setTopFromStack()
{
    mTopStone = (mCount == 0 ? Stone::Blank : (mStack & 1 << (mCount - 1)) ? Stone::BlackFlat : Stone::WhiteFlat);
}

void Square::checkValid()
{
    bool topStoneIsBlack = mTopStone & StoneBits::Black;
    bool topOfStackIsBlack = (mStack & 1 << (mCount - 1));
    assert(topStoneIsBlack == topOfStackIsBlack);
}
