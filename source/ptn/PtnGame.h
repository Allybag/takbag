#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>

#include "Ptn.h"
#include "../Result.h"
#include "Node.h"

struct PtnGame
{
    std::string mFirstPlayer{"Unknown"};
    std::string mSecondPlayer{"Unknown"};
    std::string mDate{"?"};
    Result mPtnResult{Result::None};
    std::size_t mSize{0};

    void addNode(const Node& node);

    std::size_t mSizeGuess{0};
    std::size_t mTurnNum{0};

    std::vector<Node> mTurnNodes;
};

void PtnGame::addNode(const Node& node)
{
    switch (node.mType)
    {
        case NodeType::TagNode:
            if (node.mFirstToken.mValue == "Size") // Ignoring all other tags atm
                mSize = std::stoi(node.mSecondToken.mValue);
            else if (node.mFirstToken.mValue == "Player1")
                mFirstPlayer = node.mSecondToken.mValue;
            else if (node.mFirstToken.mValue == "Player2")
                mSecondPlayer = node.mSecondToken.mValue;
            else if (node.mFirstToken.mValue == "Date")
                mDate = node.mSecondToken.mValue;
            else if (node.mFirstToken.mValue == "Result")
                mPtnResult = node.mResult;
            break;
        case NodeType::TurnNode:
            assert(node.mTurnNum == mTurnNum + 1);
            mTurnNum = node.mTurnNum;
            mTurnNodes.push_back(node);
            break;
        case NodeType::ResultNode:
            assert(mPtnResult == Result::None || mPtnResult == node.mResult);
            mPtnResult = node.mResult;
            break;
        case NodeType::CommentNode:
            break;
    }
}
