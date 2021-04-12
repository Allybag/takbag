#include "Generator.h"

std::vector<PtnGame> Generator::generate(const std::vector<Node>& nodes, bool flush)
{
    std::vector<PtnGame> games;
    for (const auto& node : nodes)
    {
        switch (node.mType)
        {
            case NodeType::TagNode:
                if (mGameInProgress)
                {
                    games.push_back(generateGame());
                }
                mNodes.push_back(node);
                break;
            case NodeType::TurnNode:
                if (!mGameInProgress)
                {
                    assert(node.mTurnNum == 1);
                    mGameInProgress = true;
                }
                mNodes.push_back(node);
                break;
            case NodeType::ResultNode:
                mNodes.push_back(node);
                games.push_back(generateGame());
                break;
            case NodeType::CommentNode:
                break;
        }
    }

    if (flush && mGameInProgress)
    {
        games.push_back(generateGame());
    }

    return games;
}

PtnGame Generator::generateGame()
{
    PtnGame ptnGame;
    for (const auto& node : mNodes)
        ptnGame.addNode(node);
    mGameInProgress = false;
    mNodes.clear();
    return ptnGame;
}
