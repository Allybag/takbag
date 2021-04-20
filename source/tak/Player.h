#pragma once

#include <iostream>

enum class Player : uint8_t
{
    White,
    Black
};

inline std::ostream& operator<<(std::ostream& stream, Player player)
{
    switch (player)
    {
        case Player::White:
            stream << "White";
            break;
        case Player::Black:
            stream << "Black";
            break;
    }

    return stream;
}

template <typename Value>
struct PlayerPair
{
    Value White;
    Value Black;

    explicit PlayerPair(Value value) : PlayerPair(value, value) { }
    PlayerPair(Value white, Value black) : White(white), Black(black) { }

    PlayerPair() = delete;
    PlayerPair(const PlayerPair&) noexcept = default;
    PlayerPair(PlayerPair&&) noexcept = default;
    PlayerPair& operator=(const PlayerPair&) noexcept = default;
    PlayerPair& operator=(PlayerPair&&) noexcept = default;
    ~PlayerPair() = default;

    Value& operator[](Player player) { return player == Player::White ? White : Black; }
    const Value& operator[](Player player) const { return player == Player::White ? White : Black; }
};

namespace std
{
    template<typename ValueType>
    struct hash<PlayerPair<ValueType>>
    {
        std::size_t operator()(const PlayerPair<ValueType>& pair) const noexcept
        {
            std::size_t whiteHash = std::hash<ValueType>{}(pair.White);
            std::size_t blackHash = std::hash<ValueType>{}(pair.Black);
            return whiteHash ^ (blackHash << 1);
        }
    };
}

static_assert(std::is_trivially_copyable_v<PlayerPair<std::size_t>>);
