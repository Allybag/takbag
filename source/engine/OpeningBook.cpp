#include "OpeningBook.h"

#include "other/StringOps.h"
#include "tak/Game.h"

#include <cassert>
#include <fstream>

std::size_t calcMoveIndex(uint8_t rowIndex, uint8_t colIndex, std::size_t boardSize)
{
    return (rowIndex * boardSize) + colIndex;
}

OpeningBook::OpeningBook(const std::string& openingBookFile)
{
    if (openingBookFile.empty())
        return;

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

            PtnTurn ptnTurn(move);
            auto moveIndex = calcMoveIndex(ptnTurn.mRank, ptnTurn.mCol, boardSize);
            auto canonicalMoveIndex = applyShift(moveIndex, boardSize, canonicalShift);

            StoneType stoneType = ptnTurn.mPlacedStoneType;
            Move canonicalResponse = Move(canonicalMoveIndex, ptnTurn.mPlacedStoneType);

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

            mOpeningTable[canonicalPosition].push_back(canonicalResponse);

            game.play(move);
        }

        openingsLoaded++;
    }

    mLogger << LogLevel::Info << "Loaded " << openingsLoaded << " opening lines from file" << Flush;
    mLogger << LogLevel::Info << "Book contains " << mOpeningTable.size() << " positions" << Flush;
}
