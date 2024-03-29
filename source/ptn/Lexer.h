#pragma once

#include "Token.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

struct Lexer
{
    std::string mBuffer;
    static std::optional<Token> match(std::string_view v) noexcept;
    std::vector<Token> tokenise(const std::string& stringBuffer);
};
