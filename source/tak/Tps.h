#pragma once

#include "Position.h"

#include <sstream>

// Tak Positional System (spec: https://www.reddit.com/r/Tak/wiki/tak_positional_system)

// A specific Tag in a Ptn file
// Very straightforward, below is the TPS description of the starting position on a 5s game:
//
// [TPS "x5/x5/x5/x5/x5 1 1"]
// [TPS "x3,12,2S/x,22S,22C,11,21/121,212,12,1121C,1212S/21S,1,21,211S,12S/x,21S,2,x2 1 26"]
// OpenTag TPS "BoardRepresentation PlayerToMove TurnNum" CloseTag

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

Position positionFromTps(const std::string& tpsData)
{
    // We can split on ' ', then on '/' and finally on ','
    auto tokens = split(tpsData, ' ');
    assert(tokens.size() == 3);

    std::string board = tokens.front();
    auto ranks = split(board, '/');
    std::size_t size = ranks.size();
    assert(size >= 4 && size <= 8);

    Position position(size);
    for (std::size_t rank = 0; rank < size; ++rank) // We do a rank first as a1 = Position::mBoard[0]
    {
        std::size_t col = 0;
        auto squares = split(ranks[rank], ',');
        for (const auto& tpsSquare : squares)
        {
            if (tpsSquare.starts_with('x')) // Empty squares
            {
                assert(tpsSquare.size() <= 2);
                col += (tpsSquare.size() == 1) ? 1 : tpsSquare.back() - '0';
            }
            else
            {
                position.setSquare(col, size - rank - 1, tpsSquare);
                col += 1;
            }
        }
    }

    if (tokens[1] == "2") // Black to play
    {
        position.togglePlayer();
    }
    else
        assert(tokens[1] == "1");

    return position;
}

