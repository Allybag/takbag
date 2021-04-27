#pragma once

#include <cstdint>
#include <iostream>
#include <cassert>

enum class Direction : uint8_t
{
    Up = '+',
    Down = '-',
    Left = '<',
    Right = '>',
    None = 0
};

const Direction Directions[] = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};

inline std::ostream& operator<<(std::ostream& stream, const Direction& dir)
{
    switch (dir)
    {
        case Direction::Up:
            stream << "Up";
            break;
        case Direction::Down:
            stream << "Down";
            break;
        case Direction::Left:
            stream << "Left";
            break;
        case Direction::Right:
            stream << "Right";
            break;
        case Direction::None:
            assert(false);
            break;
    }
    return stream;
}
