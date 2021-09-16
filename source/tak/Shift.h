#pragma once

#include <array>
#include <cstdint>
#include <iostream>

enum class Shift : uint8_t
{
    Identical,
    Vertical,
    Horizontal,
    MainDiagonal, // From square 0
    OffDiagonal,
    RotateClockwise,
    RotateCounterClockwise,
    RotateTwice
};

inline std::ostream& operator<<(std::ostream& stream, Shift shift)
{
    switch (shift)
    {
    case Shift::Identical:
        stream << "Identical";
        break;
    case Shift::Vertical:
        stream << "Vertical";
        break;
    case Shift::Horizontal:
        stream << "Horizontal";
        break;
    case Shift::MainDiagonal:
        stream << "MainDiagonal";
        break;
    case Shift::OffDiagonal:
        stream << "OffDiagonal";
        break;
    case Shift::RotateClockwise:
        stream << "RotateClockwise";
        break;
    case Shift::RotateCounterClockwise:
        stream << "RotateCounterClockwise";
        break;
    case Shift::RotateTwice:
        stream << "RotateTwice";
        break;
    }

    return stream;
}
// Bloody c++ enums
extern std::array<Shift, 8> shifts;

std::size_t applyShift(std::size_t oldIndex, std::size_t size, Shift shiftType);
Shift getReverseShift(Shift shiftType);
