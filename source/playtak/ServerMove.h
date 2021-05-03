#pragma once

#include <string>
#include "other/StringOps.h"
#include <vector>

std::string ptnToServer(const std::string& ptn)
{
    std::string serverMove;
    // We only need to deal with PTN we generate, which makes things easier
    if (std::isdigit(ptn.front()))
    {
        // This is a move
        serverMove.append("M ");

        // int pieceCount = ptn[0] - '0'; // Unused
        auto rank = ptn[1];
        auto file = ptn[2];
        auto direction = ptn[3];

        std::vector<char> dropCounts;
        for (std::size_t i = 4; i < ptn.size(); ++i)
            dropCounts.push_back(ptn[i]);

        std::string sourceSquare;
        sourceSquare.push_back(rank);
        sourceSquare.push_back(file);
        serverMove.append(sourceSquare).append(" ");

        switch (direction)
        {
            case '+':
                file += dropCounts.size();
                break;
            case '-':
                file -= dropCounts.size();
                break;
            case '>':
                rank += dropCounts.size();
                break;
            case '<':
                rank -= dropCounts.size();
                break;
            default:
                assert(false);
        }

        std::string destinationSquare;
        destinationSquare.push_back(rank);
        destinationSquare.push_back(file);
        serverMove.append(destinationSquare).append(" ");

        for (const auto dropCount : dropCounts)
        {
            serverMove.push_back(dropCount);
            serverMove.push_back(' ');
        }
    }
    else
    {
        // This is a place
        serverMove.append("P ");

        auto rank = ptn[ptn.size() - 2];
        auto file = ptn[ptn.size() - 1];

        std::string placeSquare;
        placeSquare.push_back(rank);
        placeSquare.push_back(file);
        serverMove.append(placeSquare).append(" ");

        if (ptn.size() == 3)
        {
            if (ptn.front() == 'C')
                serverMove.push_back('C');
            else if (ptn.front() == 'S')
                serverMove.push_back('W'); // W for Wall
            else
                assert(false);
        }
    }

    for (auto& c : serverMove)
        c = std::toupper(c); // We want everything always upper case

    return serverMove;
}

std::string serverToPtn(const std::string& serverMove)
{
    std::string ptn;
    auto tokens = split(serverMove, ' ');
    if (tokens.front() == "M")
    {
        // This is a Move: M C2 C5 1 1 1
        int pieceCount = 0;
        for (std::size_t i = 3; i < tokens.size(); ++i)
            pieceCount += std::stoi(tokens[i]);

        ptn.append(std::to_string(pieceCount));

        auto square = tokens[1];
        ptn.push_back(std::tolower(square[0]));
        ptn.push_back(square[1]);

        auto rankDifference = tokens[2][0] - tokens[1][0];
        auto fileDifference = tokens[2][1] - tokens[1][1];

        char direction = 0;
        if (rankDifference > 0)
            direction = '>';
        else if (rankDifference < 0)
            direction = '<';
        else if (fileDifference > 0)
            direction = '+';
        else if (fileDifference < 0)
            direction = '-';
        else
            assert(false);

        ptn.push_back(direction);

        for (std::size_t i = 3; i < tokens.size(); ++i)
            ptn.append(tokens[i]);
    }
    else
    {
        // This is a Place: P C3
        assert(tokens.front() == "P");
        if (tokens.size() == 3)
        {
            auto pieceSpecifier = tokens.back();
            if (pieceSpecifier == "C")
                ptn.push_back('C');
            else if (pieceSpecifier == "W") // W for Wall
                ptn.push_back('S');
            else
                assert(false);
        }

        auto square = tokens[1];
        ptn.push_back(std::tolower(square[0]));
        ptn.push_back(square[1]);

    }
    return ptn;
}
