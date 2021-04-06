#pragma once

#include "Player.h"
#include "Square.h"
#include "Move.h"
#include "ptn.h"

#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>

static std::unordered_map<std::size_t, std::pair<std::size_t, std::size_t>> pieceCounts = {
        std::make_pair(3, std::make_pair(10, 0)),
        std::make_pair(4, std::make_pair(15, 0)),
        std::make_pair(5, std::make_pair(21, 1)),
        std::make_pair(6, std::make_pair(30, 1)),
        std::make_pair(7, std::make_pair(40, 2)), // Sometimes played with just one capStone
        std::make_pair(8, std::make_pair(50, 2)),
};

class Position
{
    const std::size_t mSize;
    std::vector<Square> mBoard;
    Player mToPlay;
    int mOpeningSwapMoves;

    PlayerPair<std::size_t> mFlatReserves;
    PlayerPair<std::size_t> mCapReserves;

public:
    explicit Position(std::size_t size);

    std::size_t size() const { return mSize; }
    const Square& operator[](std::size_t index) const { return mBoard[index]; }
    int getOffset(Direction direction) const;
    std::size_t getPtnIndex(const PtnTurn& ptn) const;

    void play(const PtnTurn& ptn);
    std::vector<Move> generateMoves() const;

    std::string print() const;

private:
    void togglePlayer() { mToPlay = (mToPlay == Player::White) ? Player::Black : Player::White; }
    void place(const Move& place);
    void move(const Move& move);

    std::vector<Move> generateOpeningMoves() const;
    void addPlaceMoves(std::size_t index, std::vector<Move>& moves) const;
    void addMoveMoves(std::size_t index, std::vector<Move>& moves) const;

    static std::vector<uint32_t> generateDropCounts(std::size_t handSize, std::size_t maxDistance, bool endsInSmash);

};

Position::Position(std::size_t size) :  mSize(size), mToPlay(Player::White), mOpeningSwapMoves(2),
                                        mFlatReserves(PlayerPair{pieceCounts[size].first}),
                                        mCapReserves(PlayerPair{pieceCounts[size].second})
{
    mBoard.resize(mSize * mSize); // Our board is a 1d array
}

std::string Position::print() const
{
    // We want the a file on the left and the 1 rank along the bottom
    // for consistency with playtak.com and ptn.ninja
    std::stringstream output;
    for (std::size_t row = 0; row < mSize; ++row)
    {
        output << "|";
        for (std::size_t col = 0; col < mSize; ++col)
        {
            output << mBoard[row * mSize + col].print();
            output << "|";
        }
        output << "\n";
    }

    output << mToPlay << " to play\n";

    for (const auto side : {Player::White, Player::Black})
    {
        std::size_t remainingFlats = mFlatReserves[side];
        std::size_t remainingCaps = mCapReserves[side];
        output << side << " has " << remainingFlats << " flat" <<  (remainingFlats == 1 ? "" : "s") << " remaining";
        output << " and " << remainingCaps << " cap" << (remainingCaps == 1 ? "" : "s") << " remaining\n";
    }

    return output.str();
}

void Position::place(const Move& place)
{
    assert(place.mIndex < mBoard.size());
    assert(mBoard[place.mIndex].mTopStone == Stone::Blank);

    bool stoneIsBlack = place.mStone & StoneBits::Black;
    bool playerIsBlack = (mToPlay == Player::Black);
    if (mOpeningSwapMoves)
    {
        assert(stoneIsBlack != playerIsBlack);
        assert(!(place.mStone & StoneBits::Standing)); // Only allowed to play flats for the first two ply
        mOpeningSwapMoves--;
    }
    else
        assert(stoneIsBlack == playerIsBlack);

    Square singleStone = Square(place.mStone, 1, stoneIsBlack ? 1 : 0);
    mBoard[place.mIndex].add(singleStone, 1);

    if (isCap(place.mStone))
    {
        assert(mCapReserves[mToPlay]);
        mCapReserves[mToPlay] -= 1;
    }
    else
    {
        assert(mFlatReserves[mToPlay]);
        mFlatReserves[mToPlay] -= 1;
    }

    togglePlayer();
}

void Position::move(const Move &move)
{
    assert(move.mIndex < mBoard.size());
    assert(move.mCount <= mSize);
    assert(mOpeningSwapMoves == 0);

    Square& source = mBoard[move.mIndex];

    assert(source.mTopStone != Stone::Blank);

    bool stoneIsBlack = source.mTopStone & StoneBits::Black;
    bool playerIsBlack = (mToPlay == Player::Black);
    assert(stoneIsBlack == playerIsBlack);
    assert(source.mCount + 1 >= move.mCount);

    const int offset = getOffset(move.mDirection);
    Square hand = Square(source, move.mCount); // Removes mCount flats from source

    uint32_t dropCountMask = 0xf; // Last four bits set
    uint8_t stonesLeftToDrop = move.mCount;
    for (int i = 0; stonesLeftToDrop != 0; ++i)
    {
        std::size_t nextIndex = move.mIndex + ((i + 1) * offset);
        assert(nextIndex < mBoard.size()); // As size_t is unsigned this also checks for negative index

        Square& nextSquare = mBoard[nextIndex];

        uint8_t dropCount = (move.mDropCounts & (dropCountMask << i * 4)) >> (i * 4);
        assert(dropCount > 0);
        assert(dropCount <= 0x8);

        nextSquare.add(hand, dropCount);

        stonesLeftToDrop -= dropCount;
    }

    togglePlayer();
}

void Position::play(const PtnTurn &ptn)
{
    std::size_t index = getPtnIndex(ptn);
    auto moves = generateMoves();
    std::cerr << "There are " << moves.size() << " legal moves" << std::endl;
    if (ptn.mType == MoveType::Place)
    {
        auto placeMove = Move(index, ptn.mTopStone);
        assert(std::find(moves.begin(), moves.end(), placeMove) != moves.cend());
        place(placeMove);
    }
    else
    {
        auto moveMove = Move(index, ptn.mCount, ptn.mDropCounts, ptn.mDirection);
        assert(std::find(moves.begin(), moves.end(), moveMove) != moves.cend());
        move(moveMove);
    }
}

std::size_t Position::getPtnIndex(const PtnTurn& ptn) const
{
    return (mSize - 1 - ptn.mCol) * mSize + ptn.mRank;
}

int Position::getOffset(Direction direction) const
{
    switch (direction) {
        case Direction::Up:
            return -1 * mSize;
        case Direction::Down:
            return mSize;
        case Direction::Left:
            return -1;
        case Direction::Right:
            return 1;
        case Direction::None:
            assert(false);
            return 0;
    }
}

std::vector<Move> Position::generateMoves() const
{
    if (mOpeningSwapMoves)
        return generateOpeningMoves();

    std::vector<Move> moves;
    for (int index = 0; index < mBoard.size(); ++index)
    {
        const Square& square = mBoard[index];
        if (square.mTopStone == Stone::Blank)
        {
            assert(square.mCount == 0 && square.mStack == 0);
            addPlaceMoves(index, moves);
        }
        else
        {
            assert(square.mCount > 0);
            bool stoneIsBlack = square.mTopStone & StoneBits::Black;
            bool playerIsBlack = (mToPlay == Player::Black);

            if (playerIsBlack == stoneIsBlack)
                addMoveMoves(index, moves);
        }
    }

    return moves;
}


void Position::addPlaceMoves(std::size_t index, std::vector<Move> &moves) const
{
    StoneBits colour = (mToPlay == Player::Black) ? StoneBits::Black : StoneBits::Stone;
    std::vector<Stone> potentialStones;
    if (mFlatReserves[mToPlay])
    {
        potentialStones.push_back(static_cast<Stone>(Stone::WhiteFlat | colour));
        potentialStones.push_back(static_cast<Stone>(Stone::WhiteWall | colour));
    }
    if (mCapReserves[mToPlay])
    {
        potentialStones.push_back(static_cast<Stone>(Stone::WhiteCap | colour));
    }

    for (const auto& stone : potentialStones)
        moves.emplace_back(index, stone);

}

void Position::addMoveMoves(std::size_t index, std::vector<Move> &moves) const
{
    const Square& square = mBoard[index];

    std::size_t maxHandSize = std::max(static_cast<std::size_t>(square.mCount), mSize);
    bool isCapStack = isCap(square.mTopStone);

    for (const auto direction : Directions)
    {
        const int offset = getOffset(direction);

        std::size_t maxDistance = mSize;
        bool endsInSmash = false;
        for (std::size_t j = 1; j <= maxHandSize; ++j)
        {
            int nextIndex = index + j * offset;
            if (nextIndex > mBoard.size())
            {
                maxDistance = j - 1;
                break;
            }

            const Square nextSquare = mBoard[nextIndex];
            if (isWall(nextSquare.mTopStone) && isCapStack)
            {
                maxDistance = j;
                endsInSmash = true;
                break;
            }

            if (nextSquare.mTopStone & StoneBits::Standing) // Either a cap stack, or a wall and we aren't a cap stack
            {
                maxDistance = j - 1;
                break;
            }
        }

        if (maxDistance == 0)
            continue;

        for (std::size_t handSize = 1; handSize <= maxHandSize; ++handSize)
        {
            auto dropCounts = generateDropCounts(handSize, maxDistance, endsInSmash);
            for (const auto dropCount : dropCounts)
                moves.emplace_back(index, handSize, dropCount, direction);
        }
    }
}

std::vector<Move> Position::generateOpeningMoves() const
{
    std::vector<Move> moves;

    // Super simple, we can play a flat of the opposite colour in any empty square
    for (std::size_t index = 0; index < mBoard.size(); ++index)
    {
        const Square& square = mBoard[index];
        if (square.mTopStone == Stone::Blank)
        {
            Stone stone = (mToPlay == Player::White) ? Stone::BlackFlat : Stone::WhiteFlat;
            assert(square.mCount == 0 && square.mStack == 0);
            moves.emplace_back(index, stone);
        }
    }

    return moves;
}

struct DropCountGenerator
{
    uint8_t mTarget;
    uint8_t mMaxDistance;
    bool mEndsInSmash;
    DropCountGenerator(uint8_t target, uint8_t maxDistance, bool endsInSmash) : mTarget(target), mMaxDistance(maxDistance), mEndsInSmash(endsInSmash) { }

    void generateDropCounts(uint8_t sum, uint8_t distance, uint32_t dropCounts, std::vector<uint32_t>& dropCountVec) const;

};

void DropCountGenerator::generateDropCounts(uint8_t sum, uint8_t distance, uint32_t dropCounts, std::vector<uint32_t> &dropCountVec) const
{
    if (sum == mTarget)
        dropCountVec.push_back(dropCounts);

    if (distance == mMaxDistance)
        return;

    if (mEndsInSmash && distance == (mMaxDistance - 1) && sum != (mTarget - 1))
        return;

    for (uint8_t i = 1; i <= (mTarget - sum); ++i)
    {
        dropCounts += (i << ((distance - 1) * 4));
        generateDropCounts(sum + i, ++distance, dropCounts, dropCountVec);
    }
}

// TODO: Might be able to make this constexpr with C++20 constexpr vectors
// Can only be called in (8 * 8 * 2 = 128) possible configurations
std::vector<uint32_t> Position::generateDropCounts(std::size_t handSize, std::size_t maxDistance, bool endsInSmash)
{
    std::vector<uint32_t> dropCountVec;

    DropCountGenerator generator(handSize, maxDistance, endsInSmash);
    generator.generateDropCounts(0, 1, 0, dropCountVec);
    return dropCountVec;
    // 1, 1, false -> 1
    // 2, 2, false -> 11 ,2
    // 3, 3, false -> 111, 12, 21, 3
    // 4, 4, false -> 1111, 112, 121, 13, 211, 22, 31, 4
    // 6, 3, false -> 114, 123, 132, 141, 15, 213, 222, 231, 24, 312, 321, 33, 411, 42, 51, 6
    // 6, 3, false -> 6, 15, 24, 33, 42, 51, 114, 123, 132, 141, 213, 222, 231, 312, 321, 411

}

