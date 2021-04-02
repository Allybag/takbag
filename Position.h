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
    assert(place.index < mBoard.size());
    assert(mBoard[place.index].topStone == Stone::Blank);

    // TODO: Deal with the annoying first turn swap rule
    assert((place.stone & StoneBits::Black) == (mToPlay == Player::Black));
    mBoard[place.index].topStone = place.stone;

    if (place.stone & StoneBits::Standing & StoneBits::Road) // Capstone
        mCapReserves[mToPlay] -= 1;
    else
        mFlatReserves[mToPlay] -= 1;

    togglePlayer();
}