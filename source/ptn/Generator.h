#pragma once

#include <optional>
#include "Parser.h"
#include "PtnGame.h"

class Generator
{
    std::vector<Node> mNodes;
    bool mGameInProgress {false};

    PtnGame generateGame();

public:
    std::vector<PtnGame> generate(const std::vector<Node>& nodes, bool flush = false);
};
