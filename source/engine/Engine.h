#pragma once

#include <string>

class Position;

class Engine
{
    std::string chooseMoveFirst(const Position& position);
    std::string chooseMoveRandom(const Position& position);
public:
    std::string chooseMove(const Position& position);
};
