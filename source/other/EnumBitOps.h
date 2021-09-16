#pragma once

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
inline constexpr auto operator|(typename std::underlying_type<Enum>::type lhs, Enum rhs) ->
    typename std::underlying_type<Enum>::type
{
    using underType = typename std::underlying_type<Enum>::type;
    return static_cast<underType>(lhs) | static_cast<underType>(rhs);
}

template <typename Enum>
inline constexpr auto operator&(typename std::underlying_type<Enum>::type lhs, Enum rhs) ->
    typename std::underlying_type<Enum>::type
{
    using underType = typename std::underlying_type<Enum>::type;
    return static_cast<underType>(lhs) & static_cast<underType>(rhs);
}
