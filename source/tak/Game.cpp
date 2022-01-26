#include "Game.h"

#ifndef LOW_MEMORY_COMPILE
#include "ptn/Generator.h"
#include "ptn/Lexer.h"
#include "ptn/Parser.h"

#include <fstream>
#endif

void Game::play(const std::string& ptnString)
{
    PtnTurn ptnTurn(ptnString);

    fillPtn(ptnTurn);

    mPosition.play(ptnTurn);

    mMoveList.push_back(ptnTurn);
    ++mPly;
}

void Game::fillPtn(PtnTurn& ptn)
{
    if (ptn.mType == MoveType::Place)
    {
        ptn.mIsWallSmash = false; // Should be unnecessary
    }
    else // ptn.mType == MoveType::Move
    {
        std::size_t index = axisToIndex(ptn.mCol, ptn.mRank, mPosition.size());

        std::size_t finalIndex = index + mPosition.getOffset(ptn.mDirection) * ptn.mDistance;
        Stone finalStone = mPosition[finalIndex].mTopStone;

        if (isWall(finalStone))
        {
            assert(isCap(mPosition[index].mTopStone));
            ptn.mIsWallSmash = true;
        }
    }
}

std::string Game::print() const
{
    if (mPosition.checkResult() != Result::None)
    {
        for (const auto& move : mMoveList)
            std::cout << move.mSourceString << " ";
        std::cout << std::endl;
    }

    return mPosition.print();
}

Game::Game(const PtnGame& ptnGame)
    : mPosition(ptnGame.mSize), mFirstPlayer(ptnGame.mFirstPlayer), mSecondPlayer(ptnGame.mSecondPlayer),
      mDate(ptnGame.mDate), mPtnResult(ptnGame.mPtnResult), mPly(1)
{
    for (const auto& ptnTurn : ptnGame.mTurnNodes)
    {
        play(ptnTurn.mFirstToken.mValue);
        if (ptnTurn.mSecondToken.mType != TokenType::End)
            play(ptnTurn.mSecondToken.mValue);
    }

    std::cout << "Game of size " << mPosition.size() << " with " << mMoveList.size() << " moves" << std::endl;
    std::cout << "Result: " << mPtnResult << std::endl;
    std::cout << print() << std::endl;
}

std::size_t Game::moveCount() const
{
    return mPosition.generateMoves().size();
}

Result Game::checkResult() const
{
    return mPosition.checkResult();
}

#ifndef LOW_MEMORY_COMPILE
// Utility function to extract all games from a PTN file containing one or more games
std::vector<Game> readGames(const std::string& ptnFilePath)
{
    std::vector<Game> games;
    Lexer lexer;
    Parser parser;
    Generator generator;
    std::ifstream fileStream(ptnFilePath);

    std::string line;
    while (getline(fileStream, line))
    {
        auto tokens = lexer.tokenise(line);
        auto nodes = parser.parse(tokens);
        auto ptnGames = generator.generate(nodes);
        for (const auto& ptnGame : ptnGames)
            games.emplace_back(ptnGame);
    }
    auto nodes = parser.flush();
    auto ptnGames = generator.generate(nodes, true);
    for (const auto& ptnGame : ptnGames)
        games.emplace_back(ptnGame);

    return games;
}

// Utility function to extract a game from a PTN file containing a single game
Game readGame(const std::string& ptnFilePath)
{
    auto games = readGames(ptnFilePath);
    assert(games.size() == 1);
    return games.front();
}
#endif
