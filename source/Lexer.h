#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "ctre.hpp"
#pragma clang diagnostic pop

#include <vector>
#include <iostream>

#include <optional>

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
    TokenType mType;
    std::string mValue;
};

std::ostream& operator<<(std::ostream& stream, const Token& token)
{
    stream << "Token of type " << token.mType << " containing " << token.mValue;
    return stream;
}

struct Lexer
{
    static constexpr auto mPattern = ctll::fixed_string{"\\s*(\\[)" // TagOpen
                                                        "|(\\])" // TagClose
                                                        "|\\s*([1-9]\\d*)\\.\\s" // PlyNum
                                                        "|\\s*([CSF]?[a-h][1-8]['!?]*)[\\s\x00]" // PlaceMove
                                                        "|\\s*([1-8]?[a-h][1-8][+-<>][1-8]*[CSF]?\\*?['!?]*)[\\s\x00]" // MoveMove
                                                        // "|\\s*([RF10](?:/2-1/2)|(?:-[RF10]))" // Result
                                                        "|\\s*((?:(?:1/2-1/2)|(?:[RF10]-[RF10])))" // Result
                                                        "|\"(.*)\"" //TagData
                                                        "|(\\w+)\\s" // TagKey
                                                        "|\\s*\\{(.*?)\\}"}; // Comment

    std::string mBuffer;


    static std::optional<Token> match(std::string_view v) noexcept;
    std::vector<Token> tokenise(const std::string& stringBuffer);

};

std::optional<Token> Lexer::match(std::string_view v) noexcept
{
    if (auto [match, tagOpen, tagClose, plyNum, placeMove, moveMove, gameResult, tagData, tagKey, comment] = ctre::match<mPattern>(v); match) {
        if (tagOpen)
            return Token{TokenType::TagOpen, std::string(tagOpen)};
        else if (tagClose)
            return Token{TokenType::TagClose, std::string(tagClose)};
        else if (tagKey)
            return Token{TokenType::TagKey, std::string(tagKey)};
        else if (tagData)
            return Token{TokenType::TagData, std::string(tagData)};
        else if (plyNum)
            return Token{TokenType::PlyNum, std::string(plyNum)};
        else if (placeMove)
            return Token{TokenType::PlaceMove, std::string(placeMove)};
        else if (moveMove)
            return Token{TokenType::MoveMove, std::string(moveMove)};
        else if (gameResult)
            return Token{TokenType::GameResult, std::string(gameResult)};
        else if (comment)
            return Token{TokenType::Comment, std::string(comment)};
    }
    return std::nullopt;
}

std::vector<Token> Lexer::tokenise(const std::string& stringBuffer)
{
    if (!mBuffer.empty())
    {
        // Assuming we go line by line, should only ever come in here because of a multi line Comment or TagData
        std::cout << "Left over: " << mBuffer << std::endl;
        assert(false);
    }

    mBuffer.append(stringBuffer);

    std::size_t startIndex = 0;
    std::size_t len = 1;
    std::vector<Token> items;
    while (startIndex != std::string::npos)
    {
        // TODO: We're literally going character by character, must be a better way
        const auto s = std::string_view(mBuffer.c_str() + startIndex, len);
        auto item = match(s);
        if (item)
        {
            startIndex += len;
            len = 1;
            items.push_back(*item);
        }
        else
            ++len;

        if (startIndex + len > mBuffer.size() + 1)
            break; // If we end abruptly with a move like "a1" need to read the null char on the end
    }

    mBuffer.erase(0, startIndex);

    return items;
}

