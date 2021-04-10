#pragma once

#include <cstddef>
#include <iostream>

enum class TokenType : uint8_t
{
    TagOpen, // [
    TagClose, // ]
    TagKey, // Site (alphanum_)
    TagData, // "PlayTak.com" (".+")
    PlyNum, // 1. (num+.)
    PlaceMove, // Cc3 (<alpha>?<alpha><num>[?!'])
    MoveMove, // 5a2>1121*''!! (<num>?<alpha><num><dir><num>*<*>[?!'])
    GameResult, // R-O (R-0F1) in various orders
    Comment, // { Player1 had to abandon the game } // ({.+})
    End, // Cannot appear in PTN file, used to tell Parser that the previous turn is over
};

std::ostream& operator<<(std::ostream& stream, TokenType tokenType)
{
    switch (tokenType)
    {
        case TokenType::TagOpen:
            stream << "Tag Open";
            break;
        case TokenType::TagClose:
            stream << "Tag Close";
            break;
        case TokenType::TagKey:
            stream << "Tag Key";
            break;
        case TokenType::TagData:
            stream << "Tag Data";
            break;
        case TokenType::PlyNum:
            stream << "Ply Num";
            break;
        case TokenType::PlaceMove:
            stream << "PlaceMove";
            break;
        case TokenType::MoveMove:
            stream << "MoveMove";
            break;
        case TokenType::GameResult:
            stream << "GameResult";
            break;
        case TokenType::Comment:
            stream << "Comment";
            break;
        case TokenType::End:
            break;
    }
    return stream;
}

struct Token {
    TokenType mType{TokenType::End};
    std::string mValue;
};

std::ostream& operator<<(std::ostream& stream, const Token& token)
{
    stream << "Token of type " << token.mType << " containing " << token.mValue;
    return stream;
}
