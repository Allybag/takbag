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

#include <random>

template <typename Container>
auto chooseRandomElement(const Container& container) -> typename Container::const_iterator
{
    static std::random_device gRandomDevice;
    static std::default_random_engine gRandomEngine{ gRandomDevice() };
    std::uniform_int_distribution<std::size_t> range(0, container.size() - 1);
    auto element = container.begin();
    auto elementNum = range(gRandomEngine);
    std::advance(element, elementNum);
    return element;
}

