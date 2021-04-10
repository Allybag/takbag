#pragma once

#include <cstddef>
#include <utility>
#include <unordered_map>

#include "Ptn.h"
#include "Lexer.h"

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

struct PtnGame
{
    std::vector<PtnTurn> mMoves;
    std::unordered_map<PtnTag, std::string> mTags;
    std::unordered_map<std::string, std::string> mUnknownTags;

    std::size_t mSize;

    PtnGame(std::stringstream& ptnStream); // A stream of lines from a PTN File
};

enum class NodeType : uint8_t
{
    TagNode,
    PlaceNode,
    MoveNode,
    ResultNode,
    CommentNode,
};

std::ostream& operator<<(std::ostream& stream, NodeType nodeType)
{
    switch (nodeType)
    {
        case NodeType::TagNode:
            stream << "Tag";
            break;
        case NodeType::PlaceNode:
            stream << "Place";
            break;
        case NodeType::MoveNode:
            stream << "Move";
            break;
        case NodeType::ResultNode:
            stream << "Result";
            break;
        case NodeType::CommentNode:
            stream << "Comment";
            break;
    }
    return stream;
}

struct Node
{
    Node(NodeType type, std::string string) : mType(type), mString(std::move(string)) { }

    NodeType mType;
    std::string mString;
};

std::ostream& operator<<(std::ostream& stream, const Node& node)
{
    stream << "Node of type " << node.mType << " with string " << node.mString;
    return stream;
}

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
    void accept(const std::vector<Token>& tokens);
    void flush();
    const std::vector<Node>& getNodes() { return mParsedNodes; }
};

void Parser::parseTag(const std::vector<Token>& tokens)
{
    if (!mRemainingTokens.empty())
        flush();

    assert(tokens[mIndex].mType == TokenType::TagOpen);
    assert(tokens[mIndex + 1].mType == TokenType::TagKey);
    assert(tokens[mIndex + 2].mType == TokenType::TagData);
    assert(tokens[mIndex + 3].mType == TokenType::TagClose);
    std::string tagKey = tokens[mIndex + 1].mValue;
    tagKey.append(":").append(tokens[mIndex + 2].mValue);
    mParsedNodes.emplace_back(NodeType::TagNode, tagKey);
    mIndex += 4;
}

void Parser::parseResult(const std::vector<Token>& tokens)
{
    assert(tokens[mIndex].mType == TokenType::GameResult);
    mParsedNodes.emplace_back(NodeType::ResultNode, tokens[mIndex].mValue);
    ++mIndex;
}

void Parser::parseComment(const std::vector<Token> &tokens)
{
    assert(tokens[mIndex].mType == TokenType::Comment);
    mParsedNodes.emplace_back(NodeType::ResultNode, tokens[mIndex].mValue);
    ++mIndex;
}

void Parser::parsePly(const std::vector<Token>& tokens)
{
    // PlyNum, 0 or more comments, PlaceMove or MoveMove, 0 or more comments, PlaceMove or MoveMove or GameResult (or nothing)
    std::size_t originalIndex = mIndex;
    assert(tokens[mIndex].mType == TokenType::PlyNum);
    int moveNum = std::stoi(tokens[mIndex].mValue);
    ++mIndex;
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
            case TokenType::TagOpen:
            case TokenType::GameResult:
            case TokenType::End:
                assert(movesRemaining == 1);
                movesRemaining = 0;
                break;
            case TokenType::PlaceMove:
                nodes.emplace_back(NodeType::PlaceNode, tokens[mIndex].mValue);
                movesRemaining -= 1;
                mIndex++;
                break;
            case TokenType::MoveMove:
                nodes.emplace_back(NodeType::MoveNode, tokens[mIndex].mValue);
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
        mParsedNodes.insert(mParsedNodes.end(), nodes.begin(), nodes.end());
    }
}

void Parser::accept(const std::vector<Token>& tokens)
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
                break;
        }
    }
}

void Parser::flush()
{
    if (mRemainingTokens.empty())
        return;

    mIndex = 0;
    mRemainingTokens.push_back(Token{TokenType::End, ""});
    parsePly(mRemainingTokens);
    mIndex = 0;
    mRemainingTokens.clear();
}
