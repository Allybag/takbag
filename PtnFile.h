#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>

#include "ptn.h"

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

void PtnFile::parsePtnTag(const std::string &ptnLine)
{
    // [Size "6"]
    std::string tagPair = ptnLine.substr(1, ptnLine.size() - 2);
}

void PtnFile::parsePtnTurn(const std::string &ptnLine)
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

void PtnFile::parsePtnResult(const std::string &ptnLine)
{
    // R-0
}

#if 0
[Site "PlayTak.com"]
[Event "Online Play"]
[Date "2021.04.03"]
[Time "18:37:17"]
[Player1 "Simmon"]
[Player2 "Ally"]
[Clock "15:0 +20"]
[Result "0-R"]
[Size "6"]
[Komi "3"]
[Flats "30"]
[Caps "1"]

1. a6 f6
2. d4 c4
3. d3 c3
4. d5 c5
5. d2 Ce3
6. c2 1e3<1
7. e2 b2
8. Cb3 b4
9. f2 d1
10. a2 1d1+1
11. d1 Sc1
12. f3 f4
13. e3 e4
14. e5 b5
15. 1d4>1 d4
16. f1 2d3>2
17. 1d1+1 1c1+1
18. 2e4>2 Sf5
19. 3d2+12 1f5-1
20. 1b3+1 3e3<3
21. 2b4>11 3f4-3
22. 4d4>13 Sf5
23. e3 1e4-1
24. e4 a5
25. 1f4-1 1f5-1
26. 4f3<4 3f4<3
27. f5 b3
28. Sb1 a1
29. 2e3>2 1f4+1
30. 2f3+11 4d3>13
31. 5e3<131 2c2+2
32. d2 d4
33. 1d5-1 1c4>1
34. 1d3+1 2e4<2
35. 3f5-3 4f3<121
36. 2f4<2 6c3>114
0-R
#endif
