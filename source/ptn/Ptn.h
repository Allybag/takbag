#pragma once

#include "../Stone.h"
#include "../Move.h"
#include <string>

struct PtnTurn
{
    MoveType mType;

    // Either square to place in, or square to pick pieces from
    uint8_t mRank;
    uint8_t mCol;


    // Only used for moves
    Direction mDirection{Direction::None};
    uint8_t mCount{0};
    uint8_t mDistance{0};
    uint32_t mDropCounts{0};

    // Set by Game
    Stone mTopStone{Stone::Blank};
    bool mIsWallSmash{false};

    std::string mSourceString; // Can be handy to read a file in and write the same file out

    PtnTurn(const std::string& sourceString);
    std::string canonicalString() const;
};


PtnTurn::PtnTurn(const std::string &sourceString) : mSourceString(sourceString)
{
    // Spec is here: https://ustak.org/portable-tak-notation/

    // Simplest rule to check if place: ends in a number, and is 3 or fewer chars
    mType = (std::isdigit(sourceString[sourceString.size() - 1]) && sourceString.size() <= 3) ? MoveType::Place : MoveType::Move;

    if (mType == MoveType::Place)
    {
        assert(sourceString.size() == 2 || sourceString.size() == 3);

        mDirection = Direction::None;
        mCount = 0;
        mDropCounts = 0;
        mIsWallSmash = false;

        auto stoneBits = static_cast<uint8_t>(StoneBits::Stone);
        if (sourceString.size() == 3)
        {
            switch (std::tolower(sourceString[0]))
            {
                case 'c':
                    stoneBits = stoneBits | StoneBits::Standing | StoneBits::Road;
                    break;
                case 'f':
                    stoneBits = stoneBits | StoneBits::Road;
                    break;
                case 's':
                    stoneBits = stoneBits | StoneBits::Standing;
                    break;
                default:
                    assert(false);
            }
        }
        else
            stoneBits = stoneBits | StoneBits::Road; // If not specified the stone is just a flat

        mTopStone = static_cast<Stone>(stoneBits);
        mRank = std::tolower(sourceString[sourceString.size() - 2]) - 'a';
        mCol = sourceString[sourceString.size() - 1] - '1'; // The 1 rank is the first
        assert(mRank < 8 && mCol < 8);
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
        mRank = std::tolower(sourceString[currentIndex++]) - 'a';
        mCol = sourceString[currentIndex++] - '1'; // The 1 rank is the first
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
