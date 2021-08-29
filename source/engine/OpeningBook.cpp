#include "OpeningBook.h"

#include "other/StringOps.h"
#include "tak/Game.h"

#include <fstream>
#include <cassert>

std::size_t calcMoveIndex(uint8_t rowIndex, uint8_t colIndex, std::size_t boardSize)
{
    return (rowIndex * boardSize) + colIndex;
}

OpeningBook::OpeningBook(const std::string& openingBookFile)
{
    // TODO: Obviously we don't want to hardcode board size
    std::size_t boardSize = 6;
    std::ifstream fileStream(openingBookFile);

    int openingsLoaded = 0;
    std::string line;
    while (getline(fileStream, line))
    {
        auto moves = split(line, ' ');
        if (moves.empty())
            continue;

        Game game(boardSize); // Komi doesn't matter here yet
        for (const auto& move : moves)
        {
            Position position = game.getPosition();
            Shift canonicalShift = position.getCanonicalShift();
            Position canonicalPosition = position.shift(canonicalShift);

            // This is pretty dubious, I want to redo the whole PtnTurn thing which is always causing hassle
            PtnTurn ptnTurn(move);
            auto moveIndex = calcMoveIndex(ptnTurn.mRank, ptnTurn.mCol, boardSize);
            auto canonicalMoveIndex = applyShift(moveIndex, boardSize, canonicalShift);

            auto colour = game.getPosition().getPlayer();
            if (game.getPosition().isInOpeningSwap())
                colour = (colour == Player::White ? Player::Black : Player::White);

            auto stone = static_cast<Stone>(ptnTurn.mTopStone | (colour == Player::Black ? StoneBits::Black : StoneBits::Stone));
            Move canonicalResponse = Move(canonicalMoveIndex, stone);

            bool moveIsValid = false;
            for (const auto response : canonicalPosition.generateMoves())
            {
                if (response == canonicalResponse)
                {
                    moveIsValid = true;
                    break;
                }
            }
            assert(moveIsValid);

            mOpeningTable[canonicalPosition].insert(canonicalResponse);

            game.play(move);
        }

        openingsLoaded++;
    }

    mLogger << LogLevel::Info << "Loaded " << openingsLoaded << " openings from file" << Flush;
}