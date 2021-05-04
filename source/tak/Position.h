#pragma once

#include "Player.h"
#include "Square.h"
#include "Move.h"
#include "Result.h"
#include "ptn/Ptn.h"

#include <vector>
#include <unordered_map>
#include <functional>

#include <array>
#include <type_traits>

static std::unordered_map<std::size_t, std::pair<uint8_t, uint8_t>> pieceCounts = {
        std::make_pair(3, std::make_pair(10, 0)),
        std::make_pair(4, std::make_pair(15, 0)),
        std::make_pair(5, std::make_pair(21, 1)),
        std::make_pair(6, std::make_pair(30, 1)),
        std::make_pair(7, std::make_pair(40, 2)), // Sometimes played with just one capStone
        std::make_pair(8, std::make_pair(50, 2)),
};

class Position
{
    std::array<Square, 64> mBoard; // TODO: Template the array size rather than wasting space
    PlayerPair<uint8_t> mFlatReserves;
    PlayerPair<uint8_t> mCapReserves;
    uint8_t mSize;
    uint8_t mOpeningSwapMoves;
    Player mToPlay;

    // Optimisations
    inline static std::unordered_map<std::size_t, std::vector<std::size_t>> mNeighbourMap;

    void place(const Move& place);
    void move(const Move& move);
public:
    explicit Position(std::size_t size);

    Position(const Position&) noexcept = default;
    Position(Position&&) noexcept = default;
    Position& operator=(const Position& other) noexcept = default;
    Position& operator=(Position&& other) noexcept = default;
    ~Position() = default;

    std::size_t size() const { return mSize; }
    const Square& operator[](std::size_t index) const { assert(index < mSize * mSize); return mBoard[index]; }
    int getOffset(Direction direction) const;

    void play(const PtnTurn& ptn);
    void play(const Move& move);
    std::vector<Move> generateMoves() const;

    std::string print() const;

    Result checkResult() const;

    void setSquare(std::size_t col, std::size_t rank, const std::string& tpsSquare);
    void togglePlayer() { mToPlay = (mToPlay == Player::White) ? Player::Black : Player::White; }
    Player getPlayer() const { return mToPlay; }
    PlayerPair<std::size_t> checkFlatCount() const;
    PlayerPair<uint8_t> getReserveCount() const { return mFlatReserves; }

    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;

private:

    std::vector<Move> generateOpeningMoves() const;
    void addPlaceMoves(std::size_t index, std::vector<Move>& moves) const;
    void addMoveMoves(std::size_t index, std::vector<Move>& moves) const;

    static std::vector<uint32_t> generateDropCounts(std::size_t handSize, std::size_t maxDistance, bool endsInSmash);
    std::vector<std::size_t> getNeighbours(std::size_t index) const;
    bool checkConnectsOppositeEdges(const std::vector<std::size_t>& island) const;

    Result checkRoadWin() const;
    Result checkFlatWin() const;

};

namespace std
{
    template <>
    struct hash<Position>
    {
        std::size_t operator()(const Position& pos) const
        {
            std::size_t sizeHash = std::hash<std::size_t>{}(pos.size());
            std::size_t playerHash = std::hash<Player>{}(pos.getPlayer());

            // We won't hash flatReserves or capReservers, as for a given mBoard they should never differ

            std::size_t squareCount = pos.size() * pos.size();
            std::size_t boardHash = squareCount;
            for (std::size_t index = 0; index < squareCount; ++index)
            {
                auto square = pos[index];
                boardHash ^= std::hash<Square>()(square);
            }
            return boardHash ^ (playerHash << 1) ^ (sizeHash << 2);
        }
    };
}

static_assert(std::is_trivially_copyable_v<Position>);
static_assert(sizeof(Position) == 520); // A bit big...
