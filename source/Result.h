#pragma once

#include <cstddef>
#include <iostream>

#include "Stone.h"

// TODO: Not sure what I'm doing here with stone bits makes too much sense...
enum class Result : uint8_t
{
    None,
    WhiteRoad = 0 | StoneBits::Road,
    WhiteFlat = 0 | StoneBits::Stone,
    WhiteOther = 0 | StoneBits::Standing,
    BlackRoad = WhiteRoad | StoneBits::Black,
    BlackFlat = WhiteFlat | StoneBits::Black,
    BlackOther = WhiteOther | StoneBits::Black,
    Draw
};

std::ostream& operator<<(std::ostream& stream, Result result)
{
    switch (result)
    {
        case Result::WhiteRoad:
            stream << "R-O";
            break;
        case Result::WhiteFlat:
            stream << "F-O";
            break;
        case Result::WhiteOther:
            stream << "1-O";
            break;
        case Result::BlackRoad:
            stream << "0-R";
            break;
        case Result::BlackFlat:
            stream << "0-F";
            break;
        case Result::BlackOther:
            stream << "0-1";
            break;
        case Result::Draw:
            stream << "1/2-1/2";
            break;
        case Result::None:
            stream << "0-0";
            break;
    }

    return stream;
}

