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

bool resultIsAWin(Player colour, Result result)
{
    if ((colour == Player::Black && result & StoneBits::Black) ||
        (colour == Player::White && !(result & StoneBits::Black)))
        return true;

    return false; // Draws count as losses
}

Move monteCarloTreeSearch(const Position& position, int maxSeconds = 1)
{
    Logger logger("MonteCarlo");

    using namespace std::chrono;
    auto endTime = steady_clock::now() + std::chrono::seconds(maxSeconds);
    std::unordered_map<Position, Node*> nodes{};
    auto root = new Node();
    nodes[position] = root;
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
            auto moves = nextPosition.generateMoves();
            auto move = *chooseRandomElement(moves);
            nextPosition.play(move);

            auto result = nextPosition.checkResult();
            if (result != Result::None)
            {
                bool wonGame = resultIsAWin(colour, result);

                // If this was an immediate win or loss
                if (parent == root)
                {
                    if (wonGame)
                        return move; // We can win immediately, wahey!
                    else
                    {
                        // This move actually gave the opponent the win!
                        // Let's just reset and hope this doesn't happen again
                        nextPosition = position;
                    }
                }
                else
                {
                    // Backpropogate the result
                    while (parent->mParent != nullptr)
                    {
                        ++(parent->mPlayCount);
                        if (wonGame)
                            ++(parent->mWinCount);
                        parent = parent->mParent;
                    }
                    ++nodeCount;
                }
            }
        }

        // Expansion
        auto* node = new Node(parent);
        nodes[nextPosition] = node;

        // Rollout
        auto result = Result::None; // We checked for this earlier
        while (result == Result::None)
        {
            auto moves = nextPosition.generateMoves();
            auto move = *chooseRandomElement(moves);

            // To try and keep pointless shuffling to a minimum, we'll ignore moving one piece onto an empty square
            if (move.mType == MoveType::Move && move.mCount == 1)
            {
                auto offset = position.getOffset(move.mDirection);
                if (position[move.mIndex + offset].mCount == 0)
                    continue;
            }

            nextPosition.play(move);
            result = nextPosition.checkResult();
        }

        bool wonGame = resultIsAWin(colour, result);

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

    return *bestMove;
}
