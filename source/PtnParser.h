#pragma once

#include <cstddef>
#include <utility>
#include <unordered_map>

#include "Ptn.h"
#include "Lexer.h"
#include "Result.h"

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
};

enum class NodeType : uint8_t
{
    TagNode,
    TurnNode,
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
        case NodeType::TurnNode:
            stream << "Turn";
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
    NodeType mType;

    // Only for Results
    Result mResult{Result::None};

    // Only for Turns
    std::size_t mTurnNum{0};

    // Used for Tags, Moves and Comments
    Token mFirstToken; // TagKey, FirstMove or comment depending on mType
    Token mSecondToken; // TagData or SecondMove depending on myType

    Node(Token tagKey, Token tagData) : mType(NodeType::TagNode), mFirstToken(std::move(tagKey)), mSecondToken(std::move(tagData)) { }
    Node(std::size_t turnNum, Token firstMove) : mType(NodeType::TurnNode), mTurnNum(turnNum), mFirstToken(std::move(firstMove)) { }
    Node(std::size_t turnNum, Token firstMove, Token secondMove) : mType(NodeType::TurnNode), mTurnNum(turnNum), mFirstToken(std::move(firstMove)), mSecondToken(std::move(secondMove)) { }
    Node(Result result) : mType(NodeType::ResultNode), mResult(result) { }
    Node(Token comment) : mType(NodeType::CommentNode), mFirstToken(std::move(comment)) { }
};

std::ostream& operator<<(std::ostream& stream, const Node& node)
{
    stream << node.mType << " Node:";
    if (node.mResult != Result::None)
        stream << " Result: " << node.mResult;
    if (node.mTurnNum != 0)
        stream << " TurnNum: " << node.mTurnNum;
    if (node.mFirstToken.mType != TokenType::End)
    {
        assert(node.mType != NodeType::ResultNode);
        stream << " " << node.mFirstToken;
    }
    if (node.mSecondToken.mType != TokenType::End)
    {
        assert(node.mType == NodeType::TagNode || node.mType == NodeType::TurnNode);
        stream << " and " << node.mSecondToken;
    }
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
    mParsedNodes.emplace_back(tokens[mIndex + 1], tokens[mIndex + 2]);
    mIndex += 4;
}

void Parser::parseResult(const std::vector<Token>& tokens)
{
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
            case TokenType::TagOpen:
            case TokenType::GameResult:
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
