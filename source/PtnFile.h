#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>

#include "Ptn.h"

enum class PtnTag
{
    Player1, // White
    Player2, // Black
    Size,
    Clock,
    Komi,
    Flats,
    Caps,
    Result,
    Date,
    Time,
    Site,
    Event
};

struct PtnFile
{
    std::vector<PtnTurn> mMoves;
    std::unordered_map<PtnTag, std::string> mTags;
    std::unordered_map<std::string, std::string> mUnknownTags;

    std::size_t mSize;

    void parsePtnTag(const std::string& ptnLine);
    void parsePtnTurn(const std::string& ptnLine);
    void parsePtnResult(const std::string& ptnLine);
    PtnFile(const std::string& ptnFilePath); // A path to a PTN File
};

PtnFile::PtnFile(const std::string& ptnFilePath)
{
    std::ifstream gameStream(ptnFilePath);
    std::string ptnLine;

    while (std::getline(gameStream, ptnLine))
    {
        std::cout << ptnLine << std::endl;
        if (ptnLine.empty())
            continue; // Blank line, whatever
        else if (ptnLine[0] == '[' && ptnLine[ptnLine.size() - 1] == ']')
            parsePtnTag(ptnLine);
        else if (std::isalnum(ptnLine[0]))
        {
            if (ptnLine[1] == '-')
                parsePtnResult(ptnLine);
            else
                parsePtnTurn(ptnLine);
        }
        else
            std::cerr << "Unknown PTN Line: " << ptnLine << std::endl;
    }

    int maxIndex = 1;
    for (const auto& ptnTurn : mMoves)
    {
        maxIndex = std::max(maxIndex, std::max(ptnTurn.mCol + 1, ptnTurn.mRank + 1)); // Could also max ptnTurn.mCount
    }

    mSize = maxIndex; // Could feasibly be wrong, but super unlikely.
}

void PtnFile::parsePtnTag(const std::string& ptnLine)
{
    // [Size "6"]
    std::string tagPair = ptnLine.substr(1, ptnLine.size() - 2);
}

void PtnFile::parsePtnTurn(const std::string& ptnLine)
{
    // 1. a6 f6

    std::stringstream turnStream(ptnLine);
    std::string token;

    // 1st token, moveNum.
    turnStream >> token;
    assert(token[token.size() - 1] == '.');

    // 2nd token, white's move
    turnStream >> token;
    PtnTurn whiteTurn(token);
    mMoves.push_back(whiteTurn);

    // 3rd token, could either be missing, or result, or black's move
    if (turnStream >> token)
    {
        if (token[1] == '-') // Result
            parsePtnResult(token);

        PtnTurn blackTurn(token);
        mMoves.push_back(blackTurn);
    }

}

void PtnFile::parsePtnResult(const std::string& ptnLine)
{
    // R-0
    std::string unused(ptnLine);
}
