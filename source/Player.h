#pragma once

#include <iostream>

enum class Player
{
    White,
    Black
};

std::ostream& operator<<(std::ostream& stream, Player player)
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
