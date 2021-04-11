#pragma once

#include "Position.h"
#include "Ptn.h"
#include "ptn/PtnGame.h"
#include "ptn/Token.h"
#include "ptn/Node.h"
#include "ptn/Generator.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

class Game
{
    Position mPosition;

    const std::string mFirstPlayer{"Unknown"};
    const std::string mSecondPlayer{"Unknown"};
    const std::string mDate{"?"};
    const Result mPtnResult{Result::None};

    std::vector<PtnTurn> mMoveList;
    std::size_t mPly;

    std::unordered_map<std::string, std::string> mUnknownTags;

    void fillPtn(PtnTurn& ptn);

    void checkPtnResult() const;
public:
    explicit Game(std::size_t size) : mPosition(size), mMoveList{}, mPly(1) { }
    explicit Game(const PtnGame& ptnGame);
    void play(const std::string& ptnString);
    std::string print() const;
    std::size_t moveCount() const;
    Result checkResult() const;
};

void Game::play(const std::string& ptnString)
{
    PtnTurn ptnTurn(ptnString);

    fillPtn(ptnTurn);

    mPosition.play(ptnTurn);

    mMoveList.push_back(ptnTurn);
    ++mPly;
}
void Game::fillPtn(PtnTurn &ptn)
{
    // Only have to set two fields: mTopStone and mIsWallSmash
    StoneBits colour = StoneBits::Stone;
    if (mPly > 2)
        colour = (mPly % 2 == 0) ? StoneBits::Black : StoneBits::Stone;
    else
        colour = (mPly % 2 != 0) ? StoneBits::Black : StoneBits::Stone;

    if (ptn.mType == MoveType::Place)
    {
        ptn.mTopStone = static_cast<Stone>(ptn.mTopStone | colour);
        ptn.mIsWallSmash = false; // Should be unnecessary
    }
    else // ptn.mType == MoveType::Move
    {
        std::size_t index = mPosition.getPtnIndex(ptn);
        ptn.mTopStone = mPosition[index].mTopStone;

        std::size_t finalIndex = index + mPosition.getOffset(ptn.mDirection) * ptn.mDistance;
        Stone finalStone = mPosition[finalIndex].mTopStone;

        if (isWall(finalStone))
        {
            assert(isCap(ptn.mTopStone));
            ptn.mIsWallSmash = true;
        }
    }
}

std::string Game::print() const
{
    return mPosition.print();
}

Game::Game(const PtnGame& ptnGame) : mPosition(ptnGame.mSize), mFirstPlayer(ptnGame.mFirstPlayer), mSecondPlayer(ptnGame.mSecondPlayer), mDate(ptnGame.mDate), mPtnResult(ptnGame.mPtnResult), mPly(1)
{
    for (const auto& ptnTurn : ptnGame.mTurnNodes)
    {
        play(ptnTurn.mFirstToken.mValue);
        if (ptnTurn.mSecondToken.mType != TokenType::End)
            play(ptnTurn.mSecondToken.mValue);
    }

    std::cout << "Game of size " << mPosition.size() << " with " << mMoveList.size() << " moves" << std::endl;
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
