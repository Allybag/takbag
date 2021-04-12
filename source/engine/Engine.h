#pragma once

#include <string>

class Position;

class Engine
{
public:
    std::string chooseMove(const Position& position);
};
