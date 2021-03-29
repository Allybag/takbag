#pragma once
#include <cstddef>

#include <vector>
#include <string>
#include <unordered_map>

#include "Stone.h"
#include "Player.h"

struct Square
{
    Stone topStone;
    uint8_t reserveCount;
    uint32_t stack; // A reserveCount long bitSet, 0 is white and black is flat

    std::size_t count() const
    {
        return topStone == Stone::Blank ? 0 : reserveCount + 1;
    }

    Square() : topStone(Stone::Blank), reserveCount(0), stack(0) { }
    Square(const Square&) noexcept = default;
    Square(Square&&) noexcept = default;
    Square& operator=(const Square&) noexcept = default;
    Square& operator=(Square&&) noexcept = default;
    ~Square() = default;
};

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
