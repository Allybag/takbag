#pragma once

#include <cstdint>
#include <concepts>
#include <iostream>

#include "../other/EnumBitOps.h"

enum class StoneBits : uint8_t
{
    Stone = 1 << 0,
    Black = 1 << 1,
    Road = 1 << 2,     // Road eligible pieces, flats or caps
    Standing = 1 << 3, // Top only pieces, walls or caps
};

enum class StoneType : uint8_t
{
    Blank = 0,
    Flat = StoneBits::Stone | StoneBits::Road,
    Wall = StoneBits::Stone | StoneBits::Standing,
    Cap  = StoneBits::Stone | StoneBits::Road | StoneBits::Standing
};

inline std::ostream& operator<<(std::ostream& stream, StoneType stoneType)
{
    switch (stoneType)
    {
    case StoneType::Blank:
        stream << "No Stone";
        break;
    case StoneType::Flat:
        stream << "Flat";
        break;
    case StoneType::Wall:
        stream << "Wall";
        break;
    case StoneType::Cap:
        stream << "Cap";
        break;
    }

    return stream;
}
enum class Stone : uint8_t
{
    Blank = 0,
    WhiteFlat = StoneBits::Stone | StoneBits::Road,
    BlackFlat = StoneBits::Stone | StoneBits::Road | StoneBits::Black,
    WhiteWall = StoneBits::Stone | StoneBits::Standing,
    BlackWall = StoneBits::Stone | StoneBits::Standing | StoneBits::Black,
    WhiteCap = StoneBits::Stone | StoneBits::Road | StoneBits::Standing,
    BlackCap = StoneBits::Stone | StoneBits::Road | StoneBits::Standing | StoneBits::Black,
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

// Not a great way to define a concept
template <typename StoneT> concept StoneKind = std::same_as<Stone, StoneT> || std::same_as<StoneType, StoneT>;

inline bool isFlat(StoneKind auto stone)
{
    return (stone & StoneBits::Road) && !(stone & StoneBits::Standing);
}

inline bool isWall(StoneKind auto stone)
{
    return (stone & StoneBits::Standing) && !(stone & StoneBits::Road);
}

inline bool isCap(StoneKind auto stone)
{
    return (stone & StoneBits::Standing) && (stone & StoneBits::Road);
}
