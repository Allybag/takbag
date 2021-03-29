#pragma once

#include "Stone.h"

#include <string>

struct Square
{
    Stone topStone;
    uint8_t reserveCount;
    uint32_t stack; // A reserveCount long bitSet, 0 is white and 1 is black

    Square() : topStone(Stone::Blank), reserveCount(0), stack(0) { }
    Square(const Square&) noexcept = default;
    Square(Square&&) noexcept = default;
    Square& operator=(const Square&) noexcept = default;
    Square& operator=(Square&&) noexcept = default;
    ~Square() = default;

    std::size_t count() const;
    std::string print();
};

std::size_t Square::count() const
{
    return topStone == Stone::Blank ? 0 : reserveCount + 1;
}

std::string Square::print()
{
    std::string output;
    switch (topStone)
    {
        case Stone::Blank:
            return "";
        case Stone::WhiteFlat:
            output.append("F");
            break;
        case Stone::BlackFlat:
            output.append("f");
            break;
        case Stone::WhiteWall:
            output.append("S");
            break;
        case Stone::BlackWall:
            output.append("s");
            break;
        case Stone::WhiteCap:
            output.append("C");
            break;
        case Stone::BlackCap:
            output.append("c");
            break;
    }

    for (std::size_t i = 0; i < reserveCount; ++i)
        output.append(stack & (1 >> i) ? "f" : "F");

    return output;
}
