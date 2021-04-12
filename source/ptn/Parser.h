#pragma once

#include <cstddef>
#include <utility>
#include <unordered_map>

#include "Ptn.h"
#include "Lexer.h"
#include "tak/Result.h"
#include "Node.h"

// The structure and design of this Parser is taken from the great free tutorial at
// https://www.destroyallsoftware.com/screencasts/catalog/a-compiler-from-scratch

// The PTN Spec followed is here: https://ustak.org/portable-tak-notation/
// With additional notes on tags taken from here: https://www.reddit.com/r/Tak/wiki/ptn_file_format

enum class PtnTag
{
    // Required Tags
    Player1,
    Player2,
    Size,
    Date,
    Result,

    // Optional Tags
    Clock,
    Komi,
    Flats,
    Caps,
    Time,
    Site,
    Event,
    Round
};

class Parser
{
    void parseTag(const std::vector<Token>& tokens);
    void parseResult(const std::vector<Token>& tokens);
    void parseComment(const std::vector<Token>& tokens);
    void parsePly(const std::vector<Token>& tokens);


    // std::vector<PtnGame> mGames;
    std::vector<Token> mRemainingTokens;
    std::vector<Node> mParsedNodes;
    std::size_t mIndex;

public:
    std::vector<Node> parse(const std::vector<Token>& tokens);
    std::vector<Node> flush();
    const std::vector<Node>& getNodes() { return mParsedNodes; }
};
