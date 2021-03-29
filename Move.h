#pragma once

#include "Stone.h"

struct Place
{
    std::size_t index;
    Stone stone;

    Place(std::size_t placeIndex, Stone placeStone) : index(placeIndex), stone(placeStone) { }
};

struct Move
{
};