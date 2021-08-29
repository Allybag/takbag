#pragma once

#include <cstdint>
#include <array>

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

// Bloody c++ enums
extern std::array<Shift, 8> shifts;

std::size_t applyShift(std::size_t oldIndex, std::size_t size, Shift shiftType);
Shift getReverseShift(Shift shiftType);
