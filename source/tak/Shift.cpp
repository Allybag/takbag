#include "Shift.h"

#include <cassert>

std::array<Shift, 8> shifts{Shift::Identical,
                            Shift::Vertical,
                            Shift::Horizontal,
                            Shift::MainDiagonal,
                            Shift::OffDiagonal,
                            Shift::RotateClockwise,
                            Shift::RotateCounterClockwise,
                            Shift::RotateTwice};

std::size_t applyShift(std::size_t oldIndex, std::size_t size, Shift shiftType)
{
    auto flip = [size](std::size_t index) { return size - 1 - index; };
    auto recompose = [size](std::size_t rowIndex, std::size_t colIndex) { return rowIndex * size + colIndex; };

    std::size_t rowIndex = oldIndex / size;
    std::size_t colIndex = oldIndex % size;

    switch (shiftType)
    {
    case Shift::Identical:
        return recompose(rowIndex, colIndex);
    case Shift::Vertical:
        return recompose(flip(rowIndex), colIndex);
    case Shift::Horizontal:
        return recompose(rowIndex, flip(colIndex));
    case Shift::MainDiagonal:
        return recompose(colIndex, rowIndex);
    case Shift::OffDiagonal:
        return recompose(flip(colIndex), flip(rowIndex));
    case Shift::RotateClockwise:
        return recompose(colIndex, flip(rowIndex));
    case Shift::RotateCounterClockwise:
        return recompose(flip(colIndex), rowIndex);
    case Shift::RotateTwice:
        return recompose(flip(colIndex), flip(rowIndex));
    }

    assert(false);
    return 0;
}

Shift getReverseShift(Shift shiftType)
{
    // Almost all shifts are their own inverse
    if (shiftType != Shift::RotateClockwise && shiftType != Shift::RotateCounterClockwise)
        return shiftType;

    return shiftType == Shift::RotateClockwise ? Shift::RotateCounterClockwise : Shift::RotateClockwise;
}
