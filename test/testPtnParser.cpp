#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include "PtnParser.h"

#include <optional>

enum class type {
    unknown, identifier, number
};

struct lex_item {
    type t;
    std::string_view c;
};

static constexpr auto pattern = ctll::fixed_string{"([a-z]+)|([0-9]+)"};

std::optional<lex_item> lexer(std::string_view v) noexcept {
    if (auto [m,id,num] = ctre::match<pattern>(v); m) {
        if (id) {
            return lex_item{type::identifier, id};
        } else if (num) {
            return lex_item{type::number, num};
        }
    }
    return std::nullopt;
}

int main()
{
    const std::string bag = "7";
    auto item = lexer(bag);
    std::cout << item->c << std::endl;

}