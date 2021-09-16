#pragma once

#include "Node.h"
#include "PtnGame.h"

#include <vector>

class Generator
{
    std::vector<Node> mNodes;
    bool mGameInProgress{false};

    PtnGame generateGame();

public:
    std::vector<PtnGame> generate(const std::vector<Node>& nodes, bool flush = false);
};
