#pragma once

#include "Position.h"
#include "ptn.h"

class Game
{
    Position mPosition;
    std::vector<PtnTurn> mMoveList;
    std::size_t mPly;

    void fillPtn(PtnTurn& ptn);

public:
    explicit Game(std::size_t size) : mPosition(size), mMoveList{}, mPly(1) { }
    void play(const std::string& ptnString);
    std::string print() const;
};

void Game::play(const std::string& ptnString)
{
    PtnTurn ptnTurn(ptnString);

    fillPtn(ptnTurn);
    mPosition.play(ptnTurn);

    mMoveList.push_back(ptnTurn);
    ++mPly;
}
void Game::fillPtn(PtnTurn &ptn)
{
    // Only have to set two fields: mTopStone and mIsWallSmash
    StoneBits colour = mPly % 2 == 0 ? StoneBits::Black : StoneBits::Stone;
    if (ptn.mType == MoveType::Place)
    {
        ptn.mTopStone = static_cast<Stone>(ptn.mTopStone | colour);
        ptn.mIsWallSmash = false; // Should be unnecessary
    }
    else // ptn.mType == MoveType::Move
    {
        std::size_t index = ptn.mRank * mPosition.size() + ptn.mCol;
        ptn.mTopStone = mPosition[index].mTopStone;

        std::size_t finalIndex = index + mPosition.getOffset(ptn.mDirection) * ptn.mDistance;
        Stone finalStone = mPosition[finalIndex].mTopStone;

        bool landsOnWall = (finalStone & StoneBits::Standing) && !(finalStone & StoneBits::Road);
        if (landsOnWall)
        {
            bool topStoneIsCap = (ptn.mTopStone & StoneBits::Standing) && (ptn.mTopStone & StoneBits::Road);
            assert(topStoneIsCap);
            ptn.mIsWallSmash = true;
        }
    }
}

std::string Game::print() const
{
    return mPosition.print();
}


