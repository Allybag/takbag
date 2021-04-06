#pragma once

#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>

#include "Player.h"
#include "Square.h"
#include "Move.h"
#include "Ptn.h"
#include "Result.h"
#include "DropCountGenerator.h"

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

    Result checkResult() const;

private:
    void togglePlayer() { mToPlay = (mToPlay == Player::White) ? Player::Black : Player::White; }
    void place(const Move& place);
    void move(const Move& move);

    std::vector<Move> generateOpeningMoves() const;
    void addPlaceMoves(std::size_t index, std::vector<Move>& moves) const;
    void addMoveMoves(std::size_t index, std::vector<Move>& moves) const;

    static std::vector<uint32_t> generateDropCounts(std::size_t handSize, std::size_t maxDistance, bool endsInSmash);
    std::vector<std::size_t> getNeighbours(std::size_t index) const;
    bool checkConnectsOppositeEdges(const std::vector<std::size_t>& island) const;

    Result checkRoadWin() const;
    Result checkFlatWin() const;
    PlayerPair<std::size_t> checkFlatCount() const;

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
        output << side << " has " << remainingFlats << " flat" <<  (remainingFlats == 1 ? "" : "s");
        output << " and " << remainingCaps << " cap" << (remainingCaps == 1 ? "" : "s") << " remaining\n";
    }

    Result result = checkResult();
    if (result != Result::None)
        output << "Result: " << result << "\n";

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

    const bool stoneIsBlack = source.mTopStone & StoneBits::Black;
    const bool playerIsBlack = (mToPlay == Player::Black);
    assert(stoneIsBlack == playerIsBlack);
    assert(source.mCount + 1 >= move.mCount);

    const bool movingLaterally = (move.mDirection == Direction::Left || move.mDirection == Direction::Right);
    const int offset = getOffset(move.mDirection);
    Square hand = Square(source, move.mCount); // Removes mCount flats from source

    uint32_t dropCountMask = 0xf; // Last four bits set
    uint8_t stonesLeftToDrop = move.mCount;
    for (int i = 0; stonesLeftToDrop != 0; ++i)
    {
        std::size_t nextIndex = move.mIndex + ((i + 1) * offset);
        assert(nextIndex < mBoard.size()); // As size_t is unsigned this also checks for negative index

        if (movingLaterally)
            assert((nextIndex / mSize) == (move.mIndex / mSize)); // Stops us going off the right or left of the board

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
    std::size_t placeMoves = 0;
    std::size_t moveMoves = 0;
    for (int index = 0; index < mBoard.size(); ++index)
    {
        const Square& square = mBoard[index];
        if (square.mTopStone == Stone::Blank)
        {
            assert(square.mCount == 0 && square.mStack == 0);
            std::size_t moveCount = moves.size();
            addPlaceMoves(index, moves);
            placeMoves += moves.size() - moveCount;
        }
        else
        {
            assert(square.mCount > 0);
            bool stoneIsBlack = square.mTopStone & StoneBits::Black;
            bool playerIsBlack = (mToPlay == Player::Black);

            std::size_t moveCount = moves.size();
            if (playerIsBlack == stoneIsBlack)
                addMoveMoves(index, moves);
            moveMoves += moves.size() - moveCount;
        }
    }

    std::cout << "Generated " << placeMoves << " places and " << moveMoves << " moves" << std::endl;
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

    std::size_t maxHandSize = std::min(static_cast<std::size_t>(square.mCount), mSize);
    bool isCapStack = isCap(square.mTopStone);

    for (const auto direction : Directions)
    {
        const int offset = getOffset(direction);

        std::size_t maxDistance = mSize;
        bool endsInSmash = false;
        for (std::size_t j = 1; j <= maxHandSize; ++j)
        {
            int nextIndex = index + j * offset;
            if (nextIndex >= mBoard.size()) // Stops us going off the top or bottom of the board
            {
                maxDistance = j - 1;
                break;
            }

            if ((direction == Direction::Right || direction == Direction::Left))
                if ((nextIndex / mSize) != (index / mSize)) // Stops us going off the right or left of the board
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
            {
                moves.emplace_back(index, handSize, dropCount, direction);

#if 1
                Move moveMove(index, handSize, dropCount, direction);
                std::cout << moveMove << std::endl;
#endif

            }
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

// TODO: Might be able to make this constexpr with C++20 constexpr vectors
// Can only be called in (8 * 8 * 2 = 128) possible configurations
std::vector<uint32_t> Position::generateDropCounts(std::size_t handSize, std::size_t maxDistance, bool endsInSmash)
{
    std::vector<uint32_t> dropCountVec;

    DropCountGenerator generator(handSize, maxDistance, endsInSmash);
    generator.generateDropCounts(0, 0, 0, dropCountVec);
    return dropCountVec;

}

Result Position::checkRoadWin() const {
    // Plan: We iterate through the board, creating "islands"
    // We then look at each island, and check if it connects two sides
    // TODO: Think about and optimise this
    std::unordered_map<std::size_t, bool> squareInIsland;
    for (std::size_t index = 0; index < mBoard.size(); ++index)
    {
        Stone topStone = mBoard[index].mTopStone;
        if (topStone == Stone::Blank)
            continue;

        uint8_t colour = topStone & StoneBits::Black;
        if (squareInIsland[index])
            continue;

        // We do a bfs
        std::vector<std::size_t> parents;
        parents.push_back(index);
        std::vector<std::size_t> island;
        while (!parents.empty())
        {
            std::vector<std::size_t> children;
            for (const auto parent : parents)
            {
                island.push_back(parent);
                squareInIsland[parent] = true;
                for (const auto neighbour : getNeighbours(parent))
                {
                    if (squareInIsland[neighbour])
                        continue; // Already assigned to an island

                    Stone neighbourStone = mBoard[neighbour].mTopStone;

                    if (neighbourStone == Stone::Blank)
                        continue; // Empty
                    if ((neighbourStone & StoneBits::Black) != colour)
                        continue; // Wrong colour
                    if (!(neighbourStone & StoneBits::Road))
                        continue; // Wall

                    children.push_back(neighbour);
                }
            }
            std::swap(parents, children);
        }

        // TODO: Dragon clause
        if (checkConnectsOppositeEdges(island))
            return static_cast<Result>((topStone & StoneBits::Black) | Result::WhiteRoad);

    }
    return Result::None;
}

Result Position::checkFlatWin() const
{
    bool whiteStonesGone = (mFlatReserves.White == 0 && mCapReserves.White == 0);
    bool blackStonesGone = (mFlatReserves.Black == 0 && mCapReserves.Black == 0);

    bool boardFilled = true;
    for (const auto& square : mBoard)
    {
        if (square.mTopStone == Stone::Blank)
        {
            boardFilled = false;
            break;
        }
    }

    if (whiteStonesGone || blackStonesGone || boardFilled)
    {
        auto flatCounts = checkFlatCount();
        if (flatCounts.White > flatCounts.Black)
            return Result::WhiteFlat;
        else if (flatCounts.Black > flatCounts.White)
            return Result::BlackFlat;
        else
            return Result::Draw;
    }

    return Result::None;
}


Result Position::checkResult() const
{
    Result roadResult = checkRoadWin();
    return (roadResult != Result::None ? roadResult : checkFlatWin());
}

PlayerPair<std::size_t> Position::checkFlatCount() const
{
    PlayerPair<std::size_t> flatCounts{0};
    for (const auto& square : mBoard)
    {
        if (square.mTopStone != Stone::Blank)
        {
        Player colour = (square.mTopStone & StoneBits::Black) ? Player::Black : Player::White;
        flatCounts[colour] += 1;
        }
    }

    return flatCounts;
}

std::vector<std::size_t> Position::getNeighbours(std::size_t index) const
{
    std::vector<std::size_t> neighbours;
    for (const auto direction : Directions)
    {
        const int offset = getOffset(direction);
        const std::size_t neighbour = index + offset;
        if (neighbour >= mBoard.size())
            continue;
        if ((direction == Direction::Right || direction == Direction::Left))
            if ((neighbour / mSize) != (index / mSize)) // Stops us going off right or legt
                continue;

        neighbours.push_back(neighbour);
    }

    return neighbours;
}

bool Position::checkConnectsOppositeEdges(const std::vector<std::size_t>& island) const
{
    bool connectsTop = false;
    bool connectsBottom = false;
    bool connectsLeft = false;
    bool connectsRight = false;

    for (const auto index : island)
    {
        if (index < mSize)
            connectsBottom = true;
        if (index > mBoard.size() - mSize)
            connectsTop = true;
        if (index % mSize == 0)
            connectsLeft = true;
        if (index % mSize == mSize - 1)
            connectsRight = true;
    }

    return ((connectsLeft && connectsRight) || (connectsTop && connectsBottom));
}
