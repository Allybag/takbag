#include "Position.h"

#include "DropCountGenerator.h"

#include <sstream>
#include <cassert>
#include <bit>

static constexpr std::size_t gHighMoveCount = 256;

Position::Position(std::size_t size) :  mFlatReserves(PlayerPair{pieceCounts[size].first}),
                                        mCapReserves(PlayerPair{pieceCounts[size].second}),
                                        mSize(size), mOpeningSwapMoves(2), mToPlay(Player::White)
{
    if (mNeighbourMapSize != mSize)
    {
        mNeighbourMap.clear();
        for (std::size_t index = 0; index < mSize * mSize; ++index)
        {
            auto neighbours = getNeighbours(index);
            mNeighbourMap.push_back(neighbours);
        }
        mNeighbourMapSize = mSize;
    }
    if (mDropCountMap.empty())
    {
        // 1 1 false 1 1 true ... 1 8 false 1 8 true 2 1 false
        for (std::size_t handSize = 1; handSize <= 8; ++handSize)
            for (std::size_t maxDistance = 1; maxDistance <= 8; ++maxDistance)
                for (bool endsInSmash : {false, true})
                {
                    auto dropCounts = generateDropCounts(handSize, maxDistance, endsInSmash);
                    mDropCountMap.push_back(dropCounts);
                }
    }
}

std::string Position::print() const
{
    // We want the a file on the left and the 1 rank along the bottom
    // for consistency with playtak.com and ptn.ninja
    std::stringstream output;
    for (std::size_t row = mSize; row != 0; --row)
    {
        output << "|";
        for (std::size_t col = 0; col < mSize; ++col)
        {
            output << mBoard[(row - 1) * mSize + col].print();
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
    assert(place.mIndex < mSize * mSize);
    assert(mBoard[place.mIndex].mTopStone == Stone::Blank);

    bool stoneIsBlack = place.mStone & StoneBits::Black;
    bool playerIsBlack = (mToPlay == Player::Black);
    Player colour = mToPlay;
    if (mOpeningSwapMoves)
    {
        assert(stoneIsBlack != playerIsBlack);
        assert(!(place.mStone & StoneBits::Standing)); // Only allowed to play flats for the first two ply
        mOpeningSwapMoves--;
        colour = playerIsBlack ? Player::White : Player::Black;
    }
    else
        assert(stoneIsBlack == playerIsBlack);

    Square singleStone = Square(place.mStone, 1, stoneIsBlack ? 1 : 0);
    mBoard[place.mIndex].add(singleStone, 1);

    if (isCap(place.mStone))
    {
        assert(mCapReserves[mToPlay]);
        mCapReserves[colour] -= 1;
    }
    else
    {
        assert(mFlatReserves[mToPlay]);
        mFlatReserves[colour] -= 1;
    }

    togglePlayer();
}

void Position::move(const Move &move)
{
    assert(move.mIndex < mSize * mSize);
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

    uint8_t nextIndex = move.mIndex;
    auto dropStone = [&](uint8_t dropCount)
    {
        nextIndex += offset;

        assert(nextIndex < mSize * mSize);
        if (movingLaterally)
            assert((nextIndex / mSize) == (move.mIndex / mSize)); // Stops us going off the right or left of the board

        Square& nextSquare = mBoard[nextIndex];
        nextSquare.add(hand, dropCount);
    };
    move.forEachStone(dropStone);

    togglePlayer();
}

void Position::play(const PtnTurn &ptn)
{
    std::size_t index = axisToIndex(ptn.mCol, ptn.mRank, mSize);
    auto chosenMove = (ptn.mType == MoveType::Place) ? Move(index, ptn.mTopStone) : Move(index, ptn.mCount, ptn.mDropCounts, ptn.mDirection);
    play(chosenMove);
}

void Position::play(const Move& chosenMove)
{
    if (chosenMove.mType == MoveType::Place)
        place(chosenMove);
    else
        move(chosenMove);
}

int Position::getOffset(Direction direction) const
{
    switch (direction) {
        case Direction::Up:
            return mSize;
        case Direction::Down:
            return -1 * mSize;
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
    moves.reserve(gHighMoveCount);

    for (int index = 0; index < mSize * mSize; ++index)
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

    if (mCapReserves[mToPlay])
    {
        moves.emplace_back(index, static_cast<Stone>(Stone::WhiteCap | colour));
    }

    if (mFlatReserves[mToPlay])
    {
        moves.emplace_back(index, static_cast<Stone>(Stone::WhiteFlat | colour));
        moves.emplace_back(index, static_cast<Stone>(Stone::WhiteWall | colour));
    }
}


void Position::addMoveMoves(std::size_t index, std::vector<Move> &moves) const
{
    const Square& square = mBoard[index];

    auto maxHandSize = std::min(square.mCount, mSize);
    bool isCapStack = isCap(square.mTopStone);

    for (const auto direction : Directions)
    {
        const int offset = getOffset(direction);

        uint8_t maxDistance = mSize;
        bool endsInSmash = false;
        for (int j = 1; j <= maxHandSize; ++j)
        {
            uint8_t nextIndex = index + j * offset;
            if (nextIndex >= mSize * mSize) // Stops us going off the top or bottom of the board
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
            const auto dropCountIndex = (handSize - 1) * 16 + (maxDistance - 1) * 2 + endsInSmash;
            const auto& dropCounts = mDropCountMap[dropCountIndex];
            for (const auto dropCount : dropCounts)
            {
                moves.emplace_back(index, handSize, dropCount, direction);
            }
        }
    }
}

std::vector<Move> Position::generateOpeningMoves() const
{
    std::vector<Move> moves;

    // Super simple, we can play a flat of the opposite colour in any empty square
    for (std::size_t index = 0; index < mSize * mSize; ++index)
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
    Result result = Result::None;
    uint64_t squareInIsland = 0; // We use this as if it were a map, but with much faster lookup

    int nextDiagonalOffset = mSize + 1; // All roads must pass through a square on the main diagonal
    for (std::size_t index = 0; index < mSize * mSize; index += nextDiagonalOffset)
    {
        Stone topStone = mBoard[index].mTopStone;
        if (topStone == Stone::Blank || isWall(topStone))
            continue;

        uint8_t colour = topStone & StoneBits::Black;
        if (squareInIsland & (1LL << index))
            continue;

        // We do a breadth first search
        uint64_t parents = 0;
        parents |= (1LL << index);
        uint64_t island = 0;
        while (parents != 0)
        {
            uint64_t children = 0;
            while (parents != 0)
            {
                auto parentIndex = std::countr_zero(parents);
                parents -= (1LL << parentIndex);
                island |= (1LL << parentIndex);
                squareInIsland |= (1LL << parentIndex);
                for (const auto neighbour : mNeighbourMap[parentIndex])
                {
                    if (squareInIsland & (1LL << neighbour))
                        continue; // Already assigned to an island

                    Stone neighbourStone = mBoard[neighbour].mTopStone;

                    if (neighbourStone == Stone::Blank)
                        continue; // Empty
                    if ((neighbourStone & StoneBits::Black) != colour)
                        continue; // Wrong colour
                    if (isWall(neighbourStone))
                        continue; // Wall

                    children |= ( 1LL << neighbour);
                }
            }
            parents = children;
        }


        if (checkConnectsOppositeEdges(island))
        {

            bool roadIsBlack = topStone & StoneBits::Black;
            bool playerIsBlack = (mToPlay == Player::Black);
            result = static_cast<Result>((topStone & StoneBits::Black) | Result::WhiteRoad);

            if (roadIsBlack != playerIsBlack)
                return result; // Dragon clause, player should have already been toggled when he completed the road
        }
    }

    return result;
}

Result Position::checkFlatWin() const
{
    bool whiteStonesGone = (mFlatReserves.White == 0 && mCapReserves.White == 0);
    bool blackStonesGone = (mFlatReserves.Black == 0 && mCapReserves.Black == 0);

    bool boardFilled = true;
    for (std::size_t index = 0; index < mSize * mSize; ++index)
    {
        if (mBoard[index].mTopStone == Stone::Blank)
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
    for (std::size_t index = 0; index < mSize * mSize; ++index)
    {
        Stone stone = mBoard[index].mTopStone;
        if (stone != Stone::Blank && isFlat(stone))
        {
            Player colour = (stone & StoneBits::Black) ? Player::Black : Player::White;
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
        if (neighbour >= mSize * mSize)
            continue;
        if ((direction == Direction::Right || direction == Direction::Left))
            if ((neighbour / mSize) != (index / mSize)) // Stops us going off right or left
                continue;

        neighbours.push_back(neighbour);
    }

    return neighbours;
}

bool Position::checkConnectsOppositeEdges(uint64_t island) const
{
    bool connectsTop = false;
    bool connectsBottom = false;
    bool connectsLeft = false;
    bool connectsRight = false;

    while (island != 0)
    {
        auto index = std::countr_zero(island);
        island -= (1LL << index);

        if (index < mSize)
            connectsBottom = true;
        if (index >= (mSize * mSize) - mSize)
            connectsTop = true;
        if (index % mSize == 0)
            connectsLeft = true;
        if (index % mSize == mSize - 1)
            connectsRight = true;
    }

    return ((connectsLeft && connectsRight) || (connectsTop && connectsBottom));
}

void Position::setSquare(std::size_t col, std::size_t rank, const std::string& tpsSquare)
{
    assert(tpsSquare.starts_with('2') || tpsSquare.starts_with('1'));

    std::size_t index = axisToIndex(col, rank, mSize);
    assert(index < mSize * mSize);
    Square& square = mBoard[index];

    PlayerPair<std::size_t> flats{0};
    for (const char c : tpsSquare)
    {
        if (c == '1')
        {
            auto singleStone = Square(Stone::WhiteFlat);
            square.add(singleStone, 1);
            flats.White += 1;
        }
        else if (c == '2')
        {
            auto singleStone = Square(Stone::BlackFlat);
            square.add(singleStone, 1);
            flats.Black += 1;
        }
        else
        {
            assert(c == 'S' || c == 'C');
            assert(square.mTopStone != Stone::Blank);

            // We set the Standing Bit of the top stone
            auto topStone = static_cast<uint8_t>(square.mTopStone);
            topStone = topStone | StoneBits::Standing;
            if (c == 'S') // We clear the Road Bit if the top stone is a Wall
                topStone = topStone - static_cast<uint8_t>(StoneBits::Road);
            else if (c == 'C') // We add the flat back and subtract a cap if the top stone is a cap
            {
                Player colour = (square.mTopStone & StoneBits::Black) ? Player::Black : Player::White;
                flats[colour] -= 1;
                assert(mCapReserves[colour]);
                mCapReserves[colour] -= 1;
            }

            square.mTopStone = static_cast<Stone>(topStone);
        }
    }

    assert(mFlatReserves.White >= flats.White);
    assert(mFlatReserves.Black >= flats.Black);
    mFlatReserves.White -= flats.White;
    mFlatReserves.Black -= flats.Black;
}

bool Position::operator==(const Position& other) const
{
    return mSize == other.mSize && mToPlay == other.mToPlay && mBoard == other.mBoard;
}

bool Position::operator!=(const Position& other) const
{
    return !(*this == other);
}
