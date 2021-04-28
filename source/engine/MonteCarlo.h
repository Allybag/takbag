#pragma once

#include "tak/Position.h"
#include "Engine.h"
#include "log/Logger.h"

#include <unordered_map>
#include <vector>
#include <functional>
#include <cstddef>
#include <chrono>

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

std::string monteCarloTreeSearch(const Position& position, int maxSeconds = 1)
{
    Logger logger("MonteCarlo");

    using namespace std::chrono;
    auto endTime = steady_clock::now() + std::chrono::seconds(maxSeconds);
    std::unordered_map<Position, Node*> nodes{};
    nodes[position] = new Node();
    auto colour = position.getPlayer();

    std::size_t nodeCount = 0;
    while (steady_clock::now() < endTime)
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
            auto moves = nextPosition.generateMoves();
            auto move = *chooseRandomElement(moves);
            while (move.mType == MoveType::Move)
                move = *chooseRandomElement(moves);
            nextPosition.play(move);
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
        ++nodeCount;
    }

    logger << LogLevel::Info << "Searched " << nodeCount << " nodes" << Flush;

    Move* bestMove = nullptr;
    Node* bestNode = nullptr;

    auto moves = position.generateMoves();
    for (auto& move : moves)
    {
        Position nextPosition(position);
        nextPosition.play(move);

        if (!(nodes.contains(nextPosition)))
        {
            logger << LogLevel::Warn << "Unplayed top level move " << moveToPtn(move, position.size()) << Flush;
            continue;
        }

        auto* node = nodes.at(nextPosition);

        double bestNodeRatio = bestNode ? static_cast<double>(bestNode->mWinCount) / static_cast<double>(bestNode->mPlayCount) : 0.0;
        double nodeRatio = static_cast<double>(node->mWinCount) / static_cast<double>(node->mPlayCount);

        if (nodeRatio >= bestNodeRatio) // If this is the first move we've seen, pick it
        {
            std::string ptnMove = moveToPtn(move, position.size());
            logger << LogLevel::Info << ptnMove << " is new best move, wins " << node->mWinCount << " out of " << node->mPlayCount << Flush;

            bestMove = &move;
            bestNode = node;
            continue;
        }
    }

    for (auto [position, node] : nodes)
        delete node;

    return moveToPtn(*bestMove, position.size());
}
