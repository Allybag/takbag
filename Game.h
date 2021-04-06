#pragma once

#include "Position.h"
#include "ptn.h"
#include "PtnFile.h"

#include <string>
#include <vector>
#include <unordered_map>

class Game
{
    Position mPosition;
    std::vector<PtnTurn> mMoveList;
    std::size_t mPly;

    std::unordered_map<PtnTag, std::string> mTags;
    std::unordered_map<std::string, std::string> mUnknownTags;

    void fillPtn(PtnTurn& ptn);

public:
    explicit Game(std::size_t size) : mPosition(size), mMoveList{}, mPly(1) { }
    explicit Game(const PtnFile& ptnFile);
    void play(const std::string& ptnString);
    std::string print() const;
    std::size_t moveCount() const;
    Result checkResult() const;
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
    StoneBits colour = StoneBits::Stone;
    if (mPly > 2)
        colour = (mPly % 2 == 0) ? StoneBits::Black : StoneBits::Stone;
    else
        colour = (mPly % 2 != 0) ? StoneBits::Black : StoneBits::Stone;

    if (ptn.mType == MoveType::Place)
    {
        ptn.mTopStone = static_cast<Stone>(ptn.mTopStone | colour);
        ptn.mIsWallSmash = false; // Should be unnecessary
    }
    else // ptn.mType == MoveType::Move
    {
        std::size_t index = mPosition.getPtnIndex(ptn);
        ptn.mTopStone = mPosition[index].mTopStone;

        std::size_t finalIndex = index + mPosition.getOffset(ptn.mDirection) * ptn.mDistance;
        Stone finalStone = mPosition[finalIndex].mTopStone;

        if (isWall(finalStone))
        {
            assert(isCap(ptn.mTopStone));
            ptn.mIsWallSmash = true;
        }
    }
}

std::string Game::print() const
{
    return mPosition.print();
}

Game::Game(const PtnFile& ptnFile) : Game(ptnFile.mSize)
{
    for (auto ptnTurn : ptnFile.mMoves)
    {
        std::cout << "Ply: " << mPly << " Move: " << ptnTurn.mSourceString << std::endl;
        fillPtn(ptnTurn);
        play(ptnTurn.mSourceString);
        std::cout << print() << std::endl;
    }

    std::cout << "Game of size " << mPosition.size() << " with " << mMoveList.size() << " moves" << std::endl;
}

std::size_t Game::moveCount() const
{
    return mPosition.generateMoves().size();
}

Result Game::checkResult() const
{
    return mPosition.checkResult();
}
