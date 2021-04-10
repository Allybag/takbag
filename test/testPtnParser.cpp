#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include "Lexer.h"
#include "PtnParser.h"
#include "Generator.h"
#include "Game.h"

int main()
{
    auto games = readGames("games/BoardFillDraw.ptn");
}
