#pragma once

#include "Square.h"
#include <vector>
#include <unordered_map>

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

    explicit Position(std::size_t size) : mSize(size), mToPlay(Player::White),
                                          mFlatReserves(PlayerPair{pieceCounts[size].first}), mCapReserves(PlayerPair{pieceCounts[size].second}) { }
};
