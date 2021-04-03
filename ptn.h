#pragma once

#include "Stone.h"
#include "Move.h"
#include <string>

enum class MoveType
{
    Place, // (stone)(square)
    Move // (count)(square)(direction)(drop counts)(stone)
};

struct PtnTurn
{
    MoveType mType;

    // Either square to place in, or square to pick pieces from
    uint8_t mRank;
    uint8_t mCol;

    Stone mTopStone;

    // Only used for moves
    Direction mDirection;
    uint8_t mCount;
    uint32_t mDropCounts;
    bool mIsWallSmash;

    std::size_t mPly;

    std::string mSourceString; // Can be handy to read a file in and write the same file out

    PtnTurn(const std::string& sourceString, std::size_t ply);
    std::string canonicalString() const;
};


PtnTurn::PtnTurn(const std::string &sourceString, std::size_t ply) : mPly(ply), mSourceString(sourceString)
{
    // Spec is here: https://ustak.org/portable-tak-notation/

    // Simplest rule to check if place: ends in a number, and is 3 or fewer chars
    mType = (std::isdigit(sourceString[sourceString.size() - 1]) && sourceString.size() <= 3) ? MoveType::Place : MoveType::Move;
    assert(mType == MoveType::Place);

    Player player = (mPly % 2 == 0) ? Player::Black : Player::White;

    if (mType == MoveType::Place)
    {
        assert(sourceString.size() == 2 || sourceString.size() == 3);

        mDirection = Direction::None;
        mCount = 0;
        mDropCounts = 0;
        mIsWallSmash = false;

        uint8_t stoneBits = (player == Player::Black ? StoneBits::Black : StoneBits::Stone) | StoneBits::Stone;
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
}