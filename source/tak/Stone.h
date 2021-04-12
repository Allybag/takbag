#pragma once

#include <iostream>
#include <cstdint>

#include "../other/EnumBitOps.h"

enum class StoneBits : uint8_t
{
    Stone = 1 << 0,
    Black = 1 << 1,
    Road  = 1 << 2, // Road eligible pieces, flats or caps
    Standing = 1 << 3, // Top only pieces, walls or caps
};

enum class Stone : uint8_t
{
    Blank     = 0,
    WhiteFlat = StoneBits::Stone | StoneBits::Road,
    BlackFlat = StoneBits::Stone | StoneBits::Road | StoneBits::Black,
    WhiteWall = StoneBits::Stone | StoneBits::Standing,
    BlackWall = StoneBits::Stone | StoneBits::Standing | StoneBits::Black,
    WhiteCap  = StoneBits::Stone | StoneBits::Road | StoneBits::Standing,
    BlackCap  = StoneBits::Stone | StoneBits::Road | StoneBits::Standing | StoneBits::Black,
};

inline std::ostream& operator<<(std::ostream& stream, Stone stone)
{
    switch (stone)
    {
        case Stone::Blank:
            stream << "Empty";
            break;
        case Stone::WhiteFlat:
            stream << "White Flat";
            break;
        case Stone::BlackFlat:
            stream << "Black Flat";
            break;
        case Stone::WhiteWall:
            stream << "White Wall";
            break;
        case Stone::BlackWall:
            stream << "Black Wall";
            break;
        case Stone::WhiteCap:
            stream << "White Cap";
            break;
        case Stone::BlackCap:
            stream << "Black Cap";
            break;
    }
    return stream;
}

inline bool isFlat(Stone stone)
{
    return (stone & StoneBits::Road) && !(stone & StoneBits::Standing);
}

inline bool isWall(Stone stone)
{
    return (stone & StoneBits::Standing) && !(stone & StoneBits::Road);
}

inline bool isCap(Stone stone)
{
    return (stone & StoneBits::Standing) && (stone & StoneBits::Road);
}
