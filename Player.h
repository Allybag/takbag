#pragma once

enum class Player
{
    White,
    Black
};

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
