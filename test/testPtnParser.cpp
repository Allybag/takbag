#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include "PtnParser.h"

#include <optional>

enum class type {
    openTag, closeTag, tagKey, tagData,
};

struct lex_item {
    type t;
    std::string_view c;
};

static constexpr auto openTagPattern = ctll::fixed_string{"(\\[)"};
static constexpr auto closeTagPatttern = ctll::fixed_string{"(\\])"};
static constexpr auto tagKeyPattern = ctll::fixed_string{"([A-za-z0-9_]+) "};
static constexpr auto tagDataPattern = ctll::fixed_string{"\"(.*?)\")"};


static constexpr auto pattern = ctll::fixed_string{"(\\[)|(\\])|([A-Za-z]+ )|\"(.*)\""};

std::optional<lex_item> lexer(std::string_view v) noexcept {
    if (auto [m,otag,ctag,tkey,tdata] = ctre::match<pattern>(v); m) {
        if (otag) {
            return lex_item{type::openTag, otag};
        } else if (ctag) {
            return lex_item{type::closeTag, ctag};
        } else if (tkey) {
            return lex_item{type::tagKey, tkey};
        } else if (tdata) {
            return lex_item{type::tagData, tdata};
        }
    }
    return std::nullopt;
}

int main()
{
    const std::string tag = "[Site \"PlayTak.com\"]";
    std::size_t startIndex = 0;
    std::size_t len = 1;
    std::vector<lex_item> items;
    while (startIndex != std::string::npos)
    {
        const auto s = std::string_view(tag.c_str() + startIndex, len);
        std::cout << "Testing substring " << s << std::endl;
        auto item = lexer(s);
        if (item)
        {
            startIndex += len;
            std::cout << item->c << std::endl;
            len = 1;
            items.push_back(*item);
        }
        else
            ++len;

        if (len + startIndex > tag.size())
            break;
    }
    std::cout << "Tokens: \n";
    for (const auto& item : items)
        std::cout << item.c << std::endl;

}