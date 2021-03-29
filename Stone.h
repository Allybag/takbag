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