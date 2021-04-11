#pragma once

#include <cstddef>
#include <utility>
#include <unordered_map>

#include "Ptn.h"
#include "Lexer.h"
#include "Result.h"
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

void Parser::parseTag(const std::vector<Token>& tokens)
{
    if (!mRemainingTokens.empty())
        mParsedNodes = flush();

    assert(tokens[mIndex].mType == TokenType::TagOpen);
    assert(tokens[mIndex + 1].mType == TokenType::TagKey);
    assert(tokens[mIndex + 2].mType == TokenType::TagData);
    assert(tokens[mIndex + 3].mType == TokenType::TagClose);
    mParsedNodes.emplace_back(tokens[mIndex + 1], tokens[mIndex + 2]);
    mIndex += 4;
}

void Parser::parseResult(const std::vector<Token>& tokens)
{
    if (!mRemainingTokens.empty())
        mParsedNodes = flush();

    assert(tokens[mIndex].mType == TokenType::GameResult);
    Result result = resultFromString(tokens[mIndex].mValue);
    mParsedNodes.emplace_back(result);
    ++mIndex;
}

void Parser::parseComment(const std::vector<Token> &tokens)
{
    assert(tokens[mIndex].mType == TokenType::Comment);
    mParsedNodes.emplace_back(tokens[mIndex]);
    ++mIndex;
}

void Parser::parsePly(const std::vector<Token>& tokens)
{
    // PlyNum, 0 or more comments, PlaceMove or MoveMove, 0 or more comments, PlaceMove or MoveMove or GameResult (or nothing)
    std::size_t originalIndex = mIndex;
    assert(tokens[mIndex].mType == TokenType::PlyNum);
    std::size_t moveNum = std::stoi(tokens[mIndex].mValue);
    ++mIndex;

    std::vector<Token> moveTokens;
    int movesRemaining = 2;
    std::vector<Node> nodes;
    while (mIndex != tokens.size() && movesRemaining)
    {
        switch (tokens[mIndex].mType)
        {
            case TokenType::PlyNum:
            {
                int nextMoveNum = std::stoi(tokens[mIndex].mValue);
                assert(nextMoveNum == 1 || nextMoveNum == moveNum + 1);
                assert(movesRemaining == 1);
                movesRemaining = 0;
                break;
            }
            case TokenType::GameResult:
            case TokenType::TagOpen:
            case TokenType::End:
                assert(movesRemaining == 1);
                movesRemaining = 0;
                break;
            case TokenType::PlaceMove:
                moveTokens.push_back(tokens[mIndex]);
                movesRemaining -= 1;
                mIndex++;
                break;
            case TokenType::MoveMove:
                moveTokens.push_back(tokens[mIndex]);
                movesRemaining -= 1;
                mIndex++;
                break;
            case TokenType::Comment:
                parseComment(tokens);
                break;
            case TokenType::TagClose:
            case TokenType::TagKey:
            case TokenType::TagData:
                assert(false);
                break;
        }
    }

    if (movesRemaining)
    {
        for (std::size_t index = originalIndex; index != mIndex; ++index)
            mRemainingTokens.push_back(tokens[index]);
    }
    else
    {
        if (moveTokens.size() == 1)
            mParsedNodes.emplace_back(moveNum, moveTokens[0]);
        else if (moveTokens.size() == 2)
            mParsedNodes.emplace_back(moveNum, moveTokens[0], moveTokens[1]);
    }
}

std::vector<Node> Parser::parse(const std::vector<Token>& tokens)
{
    mIndex = 0;
    while (mIndex != tokens.size())
    {
        switch (tokens[mIndex].mType)
        {
            case TokenType::TagOpen:
                parseTag(tokens);
                break;
            case TokenType::PlyNum:
                parsePly(tokens);
                break;
            case TokenType::GameResult:
                parseResult(tokens);
                break;
            case TokenType::Comment:
                parseComment(tokens);
                break;
            case TokenType::TagClose:
            case TokenType::TagData:
            case TokenType::TagKey:
            case TokenType::PlaceMove:
            case TokenType::MoveMove:
            case TokenType::End:
                assert(false); // None of these tokens are valid to begin a Node
                // TODO: We actually can start with a move after a long (or multi line) comment
                break;
        }
    }

    std::vector<Node> parsedNodes;
    std::swap(parsedNodes, mParsedNodes);
    return parsedNodes;
}

std::vector<Node> Parser::flush()
{
    std::vector<Node> parsedNodes;

    if (mRemainingTokens.empty())
        return parsedNodes;

    mIndex = 0;
    mRemainingTokens.push_back(Token{TokenType::End, ""});
    parsePly(mRemainingTokens);
    mIndex = 0;
    mRemainingTokens.clear();

    std::swap(parsedNodes, mParsedNodes);
    return parsedNodes;
}
