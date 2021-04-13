#pragma once

#include "Position.h"
#include "ptn/Ptn.h"
#include "ptn/PtnGame.h"

#include <string>
#include <vector>

class Game
{
    Position mPosition;

    const std::string mFirstPlayer{"Unknown"};
    const std::string mSecondPlayer{"Unknown"};
    const std::string mDate{"?"};
    const Result mPtnResult{Result::None};

    std::vector<PtnTurn> mMoveList;
    std::size_t mPly;

    void fillPtn(PtnTurn& ptn);
public:
    explicit Game(std::size_t size) : mPosition(size), mMoveList{}, mPly(1) { }
    explicit Game(const PtnGame& ptnGame);
    void play(const std::string& ptnString);
    std::string print() const;
    std::size_t moveCount() const;
    Result checkResult() const;

    // TODO: This isn't the API we want
    const Position& getPosition() { return mPosition; }
};

std::vector<Game> readGames(const std::string& ptnFilePath);
Game readGame(const std::string& ptnFilePath);