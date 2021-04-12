#pragma once

#include <vector>
#include <optional>

#include "Token.h"

struct Lexer
{
    std::string mBuffer;


    static std::optional<Token> match(std::string_view v) noexcept;
    std::vector<Token> tokenise(const std::string& stringBuffer);
};
