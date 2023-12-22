#pragma once

#include <cassert>
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

inline std::ostream& operator<<(std::ostream& stream, Result result)
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

inline Result resultFromString(const std::string& resultString)
{
    if (resultString.size() == 3)
    {
        assert(resultString[1] == '-');
        switch (resultString[0])
        {
        case 'R':
            assert(resultString[2] == '0');
            return Result::WhiteRoad;
        case 'F':
            assert(resultString[2] == '0');
            return Result::WhiteFlat;
        case '1':
            assert(resultString[2] == '0');
            return Result::WhiteOther;
        case '0':
            switch (resultString[2])
            {
            case 'R':
                return Result::BlackRoad;
            case 'F':
                return Result::BlackFlat;
            case '1':
                return Result::BlackOther;
            case 0:
                return Result::None; // Not a real result
            default:
                assert(false);
            }
        default:
            assert(false);
        }
    }

    assert(resultString == "1/2-1/2");
    return Result::Draw;
}
