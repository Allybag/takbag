#pragma once

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
