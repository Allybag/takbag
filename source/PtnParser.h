#pragma once

#include <cstddef>
#include "ctre.hpp" // https://github.com/hanickadot/compile-time-regular-expressions
#include "Ptn.h"

// The structure and design of this Parser is taken from the great free tutorial at
// https://www.destroyallsoftware.com/screencasts/catalog/a-compiler-from-scratch

// The PTN Spec followed is here: https://ustak.org/portable-tak-notation/
// With additional notes on tags taken from here: https://www.reddit.com/r/Tak/wiki/ptn_file_format

enum class TokenTypes : uint8_t
{
    TagOpen, // [
    TagClose, // ]
    TagKey, // Site (alphanum_)
    TagData, // "PlayTak.com" (".+")
    PlyNum, // 1. (num+.)
    Ply, // 5a2>1121*''!! (alphanum+-<>*'?!) in various orders
    GameResult, // R-O (R-0F1) in various orders
    CommentOpen, // {
    CommentClose, // }
    Comment, // Anything is valid in a comment
};


#if 0
[Site "PlayTak.com"]
[Event "Online Play"]
[Date "2018.10.28"]
[Time "16:10:44"]
[Player1 "NohatCoder"]
[Player2 "fwwwwibib"]
[Clock "10:0 +20"]
[Result "R-0"]
[Size "6"]

1. a6 f6
2. d4 c4
3. d3 c3
4. d5 c5
5. d2 Ce4
6. c2 e3
7. e2 b2
8. Cb3 1e4<1
9. 1d3<1 Sd1
10. a3 1d1+1
#endif


struct Lexer
{
    // Tokens: [, ],

};

struct Parser
{

};

struct Generator
{

};