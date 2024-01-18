#pragma once

#include "HashCombine.h"
#include "Move.h"
#include "Player.h"
#include "Result.h"
#include "Shift.h"
#include "Square.h"
#include "ptn/Ptn.h"

#include <functional>
#include <unordered_map>
#include <vector>

#include <array>
#include <type_traits>

static std::unordered_map<std::size_t, std::pair<uint8_t, uint8_t>> pieceCounts = {
    std::make_pair(3, std::make_pair(10, 0)), std::make_pair(4, std::make_pair(15, 0)),
    std::make_pair(5, std::make_pair(21, 1)), std::make_pair(6, std::make_pair(30, 1)),
    std::make_pair(7, std::make_pair(40, 2)), // Sometimes played with just one capStone
    std::make_pair(8, std::make_pair(50, 2)),
};

class Position
{
    // Templating on size to reduce sizeof(Position) seems to have negligible impact
    // Probably the more compelling reason to template on size is that every now and again
    // I iterate through the board indices up to mBoard.size() instead of mSize * mSize
    // which cause memory corruption issues that make me tear my hair out
    std::array<Square, 64> mBoard;
    PlayerPair<uint8_t> mFlatReserves;
    PlayerPair<uint8_t> mCapReserves;
    uint8_t mSize;
    uint8_t mSwaps;
    int8_t mKomi; // In half points
    Player mToPlay;

    // Optimisations
    inline static std::vector<std::vector<std::uint32_t>> mDropCountMap{};
    inline static std::vector<std::vector<std::size_t>> mNeighbourMap{};
    inline static std::size_t mNeighbourMapSize{0};

    void place(const Move& place);
    void move(const Move& move);

public:
    explicit Position(std::size_t size, double komi = 0);

    Position(const Position&) noexcept = default;
    Position(Position&&) noexcept = default;
    Position& operator=(const Position& other) noexcept = default;
    Position& operator=(Position&& other) noexcept = default;
    ~Position() = default;

    std::size_t size() const
    {
        return mSize;
    }
    const Square& operator[](std::size_t index) const
    {
        assert(index < mSize * mSize);
        return mBoard[index];
    }
    int getOffset(Direction direction) const;

    void play(const PtnTurn& ptn);
    void play(const Move& move);
    MoveBuffer generateMoves() const;

    std::string print() const;

    Result checkResult() const;
    PlayerPair<std::size_t> countIslands() const;

    void setSquare(std::size_t col, std::size_t rank, const std::string& tpsSquare);
    void togglePlayer()
    {
        mToPlay = (mToPlay == Player::White) ? Player::Black : Player::White;
    }
    void setOpeningSwapMoves(std::size_t n)
    {
        mSwaps = n;
    }
    bool isInOpeningSwap() const
    {
        return mSwaps != 0;
    }
    Player getPlayer() const
    {
        return mToPlay;
    }
    double getKomi() const
    {
        return static_cast<double>(mKomi) / 2;
    }
    PlayerPair<std::size_t> checkFlatCount() const;
    Position shift(Shift shiftType) const;
    Shift getCanonicalShift() const;
    PlayerPair<uint8_t> getReserveCount() const
    {
        return mFlatReserves;
    }

    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;

private:
    void generateOpeningMoves(MoveBuffer& moves) const;
    void addPlaceMoves(std::size_t index, MoveBuffer& moves) const;
    void addMoveMoves(std::size_t index, MoveBuffer& moves) const;

    static std::vector<uint32_t> generateDropCounts(std::size_t handSize, std::size_t maxDistance, bool endsInSmash);
    std::vector<std::size_t> getNeighbours(std::size_t index) const;
    bool checkConnectsOppositeEdges(uint64_t island) const;

    Result checkRoadWin() const;
    Result checkFlatWin() const;

    void initNeighbourMap();

    void initDropCountMap();

    uint8_t calcMaxDistance(size_t index, uint8_t maxHandSize, bool isCapStack, const Direction direction) const;

    uint64_t findIsland(size_t index, uint64_t& squareInIsland) const;

    bool checkBoardFilled() const;

    uint8_t calcDistanceTillEdge(size_t index, const Direction& direction) const;
};

namespace std
{
template <> struct hash<Position>
{
    std::size_t operator()(const Position& pos) const
    {
        std::size_t sizeHash = std::hash<std::size_t>{}(pos.size());
        std::size_t playerHash = std::hash<Player>{}(pos.getPlayer());

        std::size_t squareCount = pos.size() * pos.size();
        std::size_t boardHash = squareCount;
        for (std::size_t index = 0; index < squareCount; ++index)
        {
            auto square = pos[index];
            boardHash = hash_combine(boardHash, square);
        }

        return hash_combine(sizeHash, playerHash, boardHash);
    }
};
} // namespace std

static_assert(std::is_trivially_copyable_v<Position>);
