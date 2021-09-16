#pragma once

#include "Move.h"

#include <cstddef>
#include <cstring> // std::memcpy

static constexpr std::size_t gHighMoveCount = 1024;

struct MoveBuffer
{
    std::size_t mSize;
    Move mMoves[gHighMoveCount];

    MoveBuffer() : mSize(0)
    {
        std::memset(mMoves, 0, sizeof(Move) * gHighMoveCount);
    }

    MoveBuffer(const MoveBuffer& other)
    {
        std::memcpy(this, &other, sizeof(MoveBuffer));
    }
    MoveBuffer(MoveBuffer&& other) noexcept
    {
        std::memcpy(this, &other, sizeof(MoveBuffer));
    }
    MoveBuffer& operator=(const MoveBuffer& other) noexcept
    {
        std::memcpy(this, &other, sizeof(MoveBuffer));
        return *this;
    }
    MoveBuffer& operator=(MoveBuffer&& other)
    {
        std::memcpy(this, &other, sizeof(MoveBuffer));
        return *this;
    }
    ~MoveBuffer() = default;

    // Give us iterability
    const Move* begin() const
    {
        return &mMoves[0];
    }
    const Move* end() const
    {
        return &mMoves[mSize];
    }
    using iterator = Move*;
    using const_iterator = const Move*;
    void reserve(std::size_t)
    {
    } // Matching vector's interface

    std::size_t size() const
    {
        return mSize;
    }
    bool empty() const
    {
        return mSize == 0;
    }

    void clear()
    {
        mSize = 0;
    }

    Move front() const
    {
        assert(mSize > 0);
        return mMoves[0];
    }

    void inc()
    {
        ++mSize;
    }

    void push_back(Move move)
    {
        mMoves[mSize] = move;
        ++mSize;
    }

    void emplace_back(uint8_t index, Stone stone) noexcept
    {
        mMoves[mSize].mIndex = index;
        mMoves[mSize].mStone = stone;
        mMoves[mSize].mDirection = Direction::None;
        ++mSize;
    }

    void emplace_back(uint8_t index, uint8_t handSize, uint32_t dropCount, Direction direction) noexcept
    {
        mMoves[mSize].mIndex = index;
        mMoves[mSize].mCount = handSize;
        mMoves[mSize].mDropCounts = dropCount;
        mMoves[mSize].mDirection = direction;
        ++mSize;
    }
};