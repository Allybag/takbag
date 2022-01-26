#pragma once

#include "tak/Move.h"
#include "tak/Stone.h"
#include <string>

struct PtnTurn
{
    MoveType mType;

    // Either square to place in, or square to pick pieces from
    uint8_t mCol;
    uint8_t mRank;

    // Only used for moves
    Direction mDirection{Direction::None};
    uint8_t mCount{0};
    uint8_t mDistance{0};
    uint32_t mDropCounts{0};

    // Only used for placing
    StoneType mPlacedStoneType{StoneType::Blank};

    // Set by game
    bool mIsWallSmash{false};

    std::string mSourceString; // Can be handy to read a file in and write the same file out

    PtnTurn(const std::string& sourceString);
    std::string canonicalString() const;
};

inline StoneType getStoneType(std::string_view ptn) {
    if (ptn.size() == 3)
    {
        switch (std::tolower(ptn[0]))
        {
        case 'c':
            return StoneType::Cap;
        case 'f':
            return StoneType::Flat;
        case 's':
            return StoneType::Wall;
        default:
            assert(false);
        }
    }

    return StoneType::Flat;
}

inline PtnTurn::PtnTurn(const std::string& sourceString) : mSourceString(sourceString)
{
    // Spec is here: https://ustak.org/portable-tak-notation/

    // Simplest rule to check if place: ends in a number, and is 3 or fewer chars
    mType = (std::isdigit(sourceString[sourceString.size() - 1]) && sourceString.size() <= 3) ? MoveType::Place
                                                                                              : MoveType::Move;

    if (mType == MoveType::Place)
    {
        assert(sourceString.size() == 2 || sourceString.size() == 3);

        mDirection = Direction::None;
        mCount = 0;
        mDropCounts = 0;
        mIsWallSmash = false;

        mPlacedStoneType = getStoneType(sourceString);
        mCol = std::tolower(sourceString[sourceString.size() - 2]) - 'a';
        mRank = sourceString[sourceString.size() - 1] - '1'; // The 1 rank is the first
        assert(mCol < 8 && mRank < 8);
    }
    else // mType == MoveType::Move
    {
        // Move // (count)(square)(direction)(drop counts)(stone)
        // Valid strings: a1>, 1a1>, 1a1>1, 1a1>1F
        std::size_t currentIndex = 0;
        if (std::isdigit(sourceString[currentIndex]))
        {
            mCount = sourceString[currentIndex++] - '0';
        }
        else
        {
            mCount = 1;
        }
        mCol = std::tolower(sourceString[currentIndex++]) - 'a';
        mRank = sourceString[currentIndex++] - '1'; // The 1 rank is the first
        assert(mRank < 8 && mCol < 8);

        mDirection = static_cast<Direction>(sourceString[currentIndex++]);

        if (currentIndex == sourceString.size() || !(std::isdigit(sourceString[currentIndex])))
        {
            mDropCounts = mCount;
            mDistance = 1;
            return;
        }

        uint8_t totalCount = 0;
        for (int i = 0;; ++i)
        {
            if (!(std::isdigit(sourceString[currentIndex + i])))
                break;

            int dropCount = (sourceString[currentIndex + i] - '0');
            assert(dropCount != 0);
            totalCount += dropCount;
            ++mDistance;
            mDropCounts += (dropCount << (i * 4));
        }
        assert(totalCount == mCount);
    }
}
