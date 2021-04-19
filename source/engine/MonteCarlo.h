#pragma once

#include "tak/Position.h"
#include "Engine.h"

#include <unordered_map>
#include <vector>
#include <functional>
#include <cstddef>

struct Node
{
    Node* mParent;

    std::size_t mPlayCount;
    std::size_t mWinCount; // Games are either won or not, but draws aren't common in Tak

    // Node(Position rootPosition) : mParent(nullptr), mPosition(rootPosition), mPlayCount(0), mWinCount(0) { }
    // Node(Node* parent, Position position) : mParent(parent), mPosition(position), mPlayCount(0), mWinCount(0) { }
    Node() : mParent(nullptr), mPlayCount(0), mWinCount(0) { }
    Node(Node* parent) : mParent(parent), mPlayCount(0), mWinCount(0) { }
};

std::string monteCarloTreeSearch(const Position& position)
{
    std::unordered_map<Position, Node*> nodes{};
    nodes[position] = new Node();
    auto colour = position.getPlayer();

    std::size_t nodeCount = 1000;
    while (nodeCount != 0)
    {
        Position nextPosition(position);
        Node* parent = nullptr;

        // Selection
        while (nodes.contains(nextPosition))
        {
            parent = nodes[nextPosition];
            nextPosition.play(*chooseRandomElement(nextPosition.generateMoves()));
        }

        // Expansion
        auto* node = new Node(parent);
        nodes[nextPosition] = node;

        // Rollout
        while (nextPosition.checkResult() == Result::None)
        {
            nextPosition.play(*chooseRandomElement(nextPosition.generateMoves()));
        }

        auto result = nextPosition.checkResult();
        bool wonGame = false;
        if ((colour == Player::Black && result & StoneBits::Black) ||
            (colour == Player::White && !(result & StoneBits::Black) && result != Result::Draw))
            wonGame = true;

        // Back Propogation
        while (node->mParent != nullptr)
        {
            ++(node->mPlayCount);
            if (wonGame)
                ++(node->mWinCount);
            node = node->mParent;
        }
        --nodeCount;
    }

    Move* bestMove = nullptr;
    Node* bestNode = nullptr;

    auto moves = position.generateMoves();
    for (auto& move : moves)
    {
        Position nextPosition(position);
        nextPosition.play(move);
        auto* node = nodes[nextPosition];

        if (node->mPlayCount == 0) // We expect to look at all possible moves
        {
            std::cout << "Unplayed top level move?" << std::endl;
            assert(node->mWinCount == 0);
            continue;
        }

        double bestNodeRatio = bestNode ? static_cast<double>(bestNode->mWinCount) / static_cast<double>(bestNode->mPlayCount) : 0.0;
        double nodeRatio = static_cast<double>(node->mWinCount) / static_cast<double>(node->mPlayCount);

        if (nodeRatio >= bestNodeRatio) // If this is the first move we've seen, pick it
        {
            std::string ptnMove = moveToPtn(move, position.size());
            std::cout << ptnMove << " is new best move, wins " << node->mWinCount << " out of " << node->mPlayCount << std::endl;
            bestMove = &move;
            bestNode = node;
            continue;
        }
    }

    for (auto [position, node] : nodes)
        delete node;

    return moveToPtn(*bestMove, position.size());
}
