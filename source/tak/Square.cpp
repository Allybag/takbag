#include "Square.h"

#include "log/Logger.h"
static Logger logger{"Square"};

#include <cassert>
#include <sstream>

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
        output << (mStack & (1LL << (i - 1)) ? "f" : "F");

    return output.str();
}

void Square::add(Square& source, uint8_t count)
{
    assert(!isCap(mTopStone));
    assert(count);
    assert(source.mCount >= count);

    if (mCount + count >= 32)
    {
        logger << LogLevel::Warn << "Dropping bottom 6 stones from stack of " << mCount + count << " stones" << Flush;
        mCount -= 6;
        mStack >>= 6;
    }

    // Wall smashes
    if (isWall(mTopStone))
    {
        assert(source.mCount == 1 && isCap(source.mTopStone));
        // We don't actually use mTopStone again, so we don't need to flatten it
    }

    // TODO: Test all this!
    uint32_t movingMask = (1LL << count) - 1;

    uint32_t movingStones = (source.mStack & movingMask); // Just the stones we need to move
    movingStones = movingStones << mCount;

    // Deal with this square
    mCount += count;
    mStack += movingStones;

    // Deal with the old square
    // We drop from the bottom
    source.mCount -= count;
    source.mStack >>= count;

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
    uint32_t mask = (1LL << mCount) - 1;
    mStack &= mask;
}

void Square::setTopFromStack()
{
    mTopStone = (mCount == 0 ? Stone::Blank : (mStack & 1LL << (mCount - 1)) ? Stone::BlackFlat : Stone::WhiteFlat);
}

void Square::checkValid() const
{
    bool topStoneIsBlack = mTopStone & StoneBits::Black;
    bool topOfStackIsBlack = (mStack & 1LL << (mCount - 1));
    assert(topStoneIsBlack == topOfStackIsBlack);
}

bool Square::operator==(const Square& other) const
{
    return mTopStone == other.mTopStone && mCount == other.mCount && mStack == other.mStack;
}

bool Square::operator!=(const Square& other) const
{
    return !(*this == other);
}
