#pragma once

#include "Square.h"
#include "../../external/robin_hood.h"

template <typename T>
struct Hash
{
};

template<>
struct Hash<Square>
{
    std::size_t operator()(const Square& square) const noexcept
    {
        std::size_t stoneHash = robin_hood::hash<Stone>{}(square.mTopStone);
        std::size_t countHash = robin_hood::hash<uint8_t>{}(square.mCount);
        std::size_t stackHash = robin_hood::hash<uint32_t>{}(square.mStack);
        return stoneHash ^ (countHash << 1) ^ (stackHash << 2);
    }
};

template <>
struct Hash<Position>
{
    std::size_t operator()(const Position& pos) const
    {
        std::size_t sizeHash = robin_hood::hash<uint8_t>{}(pos.size());
        std::size_t playerHash = robin_hood::hash<Player>{}(pos.getPlayer());

        // We won't hash flatReserves or capReservers, as for a given mBoard they should never differ

        std::size_t squareCount = pos.size() * pos.size();
        std::size_t boardHash = robin_hood::hash<std::size_t>{}(squareCount);
        for (std::size_t index = 0; index < squareCount; ++index)
        {
            auto square = pos[index];
            boardHash ^= (Hash<Square>{}(square) << index);
        }
        return boardHash ^ (playerHash << 1) ^ (sizeHash << 2);
    }
};
