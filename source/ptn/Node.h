#pragma once

#include <cstddef>
#include <iostream>

#include "Token.h"
#include "tak/Result.h"

enum class NodeType : uint8_t
{
    TagNode,
    TurnNode,
    ResultNode,
    CommentNode,
};

inline std::ostream& operator<<(std::ostream& stream, NodeType nodeType)
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

inline std::ostream& operator<<(std::ostream& stream, const Node& node)
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
