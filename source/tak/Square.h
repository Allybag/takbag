#pragma once

#include "HashCombine.h"
#include "Stone.h"

#include <cstddef>
#include <iostream>
#include <string>
#include <type_traits>

// Representation:
// mTopStone describes the surface stone and mStack[0]
// When we add a stone of colour c, we set mStack[mCount] = c, increment mCount and set mTopStone accordingly
// When we remove n stones, we subtract n from mCount, and set mTopStone = mStack[mCount - 1]
struct Square
{
    Stone mTopStone;
    uint8_t mCount;
    uint32_t mStack; // A mCount long bitSet, 0 is white and 1 is black

    Square() : mTopStone(Stone::Blank), mCount(0), mStack(0)
    {
    }
    explicit Square(Stone stone) : mTopStone(stone), mCount(1), mStack((stone & StoneBits::Black ? 1 : 0))
    {
    }
    Square(Stone topStone, uint8_t count, uint32_t stack) : mTopStone(topStone), mCount(count), mStack(stack)
    {
    }
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
    void checkValid() const;

    bool operator==(const Square& other) const;
    bool operator!=(const Square& other) const;
};

namespace std
{
template <> struct hash<Square>
{
    std::size_t operator()(const Square& square) const noexcept
    {
        std::size_t stoneHash = std::hash<Stone>{}(square.mTopStone);
        std::size_t countHash = std::hash<uint8_t>{}(square.mCount);
        std::size_t stackHash = std::hash<uint32_t>{}(square.mStack);
        return hash_combine(stoneHash, countHash, stackHash);
    }
};
} // namespace std

static_assert(std::is_trivially_copyable_v<Square>);
static_assert(sizeof(Square) == 8);
