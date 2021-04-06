#pragma once

#include <iostream>
#include <cstdint>
#include <type_traits>

// Let us use bitwise operations on our scoped enums
template <typename LeftEnum, typename RightEnum>
inline constexpr auto operator|(LeftEnum lhs, RightEnum rhs) -> typename std::underlying_type<LeftEnum>::type
{
    using underType = typename std::underlying_type<LeftEnum>::type;
    static_assert(std::is_same<underType, typename std::underlying_type<RightEnum>::type>::value);
    return static_cast<underType>(lhs) | static_cast<underType>(rhs);
}

template <typename LeftEnum, typename RightEnum>
inline constexpr auto operator&(LeftEnum lhs, RightEnum rhs) -> typename std::underlying_type<LeftEnum>::type
{
    using underType = typename std::underlying_type<LeftEnum>::type;
    static_assert(std::is_same<underType, typename std::underlying_type<RightEnum>::type>::value);
    return static_cast<underType>(lhs) & static_cast<underType>(rhs);
}

// These are necessary for chaining operators, so that Bits::X | Bits::Y | Bits::Z = underType(X | Y) | Bits::Z works
template <typename Enum>
inline constexpr auto operator|(typename std::underlying_type<Enum>::type lhs, Enum rhs) -> typename std::underlying_type<Enum>::type
{
    using underType = typename std::underlying_type<Enum>::type;
    return static_cast<underType>(lhs) | static_cast<underType>(rhs);
}

template <typename Enum>
inline constexpr auto operator&(typename std::underlying_type<Enum>::type lhs, Enum rhs) -> typename std::underlying_type<Enum>::type
{
    using underType = typename std::underlying_type<Enum>::type;
    return static_cast<underType>(lhs) & static_cast<underType>(rhs);
}

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

std::ostream& operator<<(std::ostream& stream, Stone stone)
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

