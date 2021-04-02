#pragma once

#include "Player.h"
#include "Square.h"
#include "Move.h"

#include <vector>
#include <unordered_map>
#include <sstream>

static std::unordered_map<std::size_t, std::pair<std::size_t, std::size_t>> pieceCounts = {
        std::make_pair(3, std::make_pair(10, 0)),
        std::make_pair(4, std::make_pair(15, 0)),
        std::make_pair(5, std::make_pair(21, 1)),
        std::make_pair(6, std::make_pair(30, 1)),
        std::make_pair(7, std::make_pair(40, 2)), // Sometimes played with just one capStone
        std::make_pair(8, std::make_pair(50, 2)),
};

class Position
{
    const std::size_t mSize;
    std::vector<Square> mBoard;
    Player mToPlay;

    PlayerPair<std::size_t> mFlatReserves;
    PlayerPair<std::size_t> mCapReserves;

public:
    explicit Position(std::size_t size);

    std::string print() const;

    void place(const Place& place);
    void move(const Move& move);

private:
    void togglePlayer() { mToPlay = (mToPlay == Player::White) ? Player::Black : Player::White; }

};

Position::Position(std::size_t size) :  mSize(size), mToPlay(Player::White), mFlatReserves(PlayerPair{pieceCounts[size].first}), mCapReserves(PlayerPair{pieceCounts[size].second})
{
    mBoard.resize(mSize * mSize); // Our board is a 1d array
}

std::string Position::print() const
{
    std::stringstream output;
    for (std::size_t row = 0; row < mSize; ++row)
    {
        output << "|";
        for (std::size_t col = 0; col < mSize; ++col)
        {
            output << mBoard[row * mSize + col].print();
            output << "|";
        }
        output << "\n";
    }

    output << mToPlay << " to play\n";

    for (const auto side : {Player::White, Player::Black})
    {
        std::size_t remainingCaps = mCapReserves[side];
        output << side << " has " << mFlatReserves[side] << " stones remaining";
        output << " and " << remainingCaps << " cap" << (remainingCaps == 1 ? "" : "s") << " remaining\n";
    }

    return output.str();
}

void Position::place(const Place& place)
{
    assert(place.mIndex < mBoard.size());
    assert(mBoard[place.mIndex].mTopStone == Stone::Blank);

    // TODO: Deal with the annoying first turn swap rule
    bool stoneIsBlack = place.mStone & StoneBits::Black;
    bool playerIsBlack = (mToPlay == Player::Black);
    assert(stoneIsBlack == playerIsBlack);
    Square singleStone = Square(place.mStone, 1, stoneIsBlack ? 1 : 0);
    mBoard[place.mIndex].add(singleStone, 1);

    if (place.mStone & StoneBits::Standing && (place.mStone & StoneBits::Road)) // Capstone
    {
        assert(mCapReserves[mToPlay]);
        mCapReserves[mToPlay] -= 1;
    }
    else
    {
        assert(mFlatReserves[mToPlay]);
        mFlatReserves[mToPlay] -= 1;
    }

    togglePlayer();
}

void Position::move(const Move &move)
{
    assert(move.mIndex < mBoard.size());

    Square& source = mBoard[move.mIndex];

    assert(source.mTopStone != Stone::Blank);

    bool stoneIsBlack = source.mTopStone & StoneBits::Black;
    bool playerIsBlack = (mToPlay == Player::Black);
    assert(stoneIsBlack == playerIsBlack);
    assert(source.mCount + 1 >= move.mCount);


    const int offset = [ &move, this ]() -> int {
                switch (move.mDirection) {
                    case Direction::Up:
                        return -1 * mSize;
                    case Direction::Down:
                        return mSize;
                    case Direction::Left:
                        return -1;
                    case Direction::Right:
                        return 1;
                } }();

    Square hand = Square(source, move.mCount); // Removes mCount flats from source

    uint32_t dropCountMask = 0xf; // Last four bits set
    uint8_t stonesLeftToDrop = move.mCount;
    for (int i = 0; stonesLeftToDrop != 0; ++i)
    {
        std::size_t nextIndex = move.mIndex + ((i + 1) * offset);
        assert(nextIndex < mBoard.size()); // As size_t is unsigned this also checks for negative index

        Square& nextSquare = mBoard[nextIndex];

        uint8_t dropCount = (move.mDropCounts & (dropCountMask << i * 4)) >> (i * 4);
        assert(dropCount > 0);
        assert(dropCount <= 0x8);

        nextSquare.add(hand, dropCount);

        stonesLeftToDrop -= dropCount;
    }

    togglePlayer();
}
