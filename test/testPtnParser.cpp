#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include "ctre.hpp"

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
};

struct Token {
    TokenType mType;
    std::string_view mValue;
};

// Oh Boy
static constexpr auto pattern = ctll::fixed_string{"\\s*(\\[)|(\\])|([A-Za-z0-9_]+\\s)|\"(.*)\"|\\s*([1-9][0-9]*)\\.\\s|\\s*([CSF]?[a-h][1-8])\\s|\\s*([1-8]?[a-h][1-8][+-<>][1-8]*[CSF]?\\*?)\\s|\\s*([RF10](?:\\2)-[RF10](?:\\2))|\\s*\\{(.*?)\\}"};

std::optional<Token> lexer(std::string_view v) noexcept {
    if (auto [match, tagOpen, tagClose, tagKey, tagData, plyNum, placeMove, moveMove, gameResult, comment] = ctre::match<pattern>(v); match) {
        if (tagOpen)
            return Token{TokenType::TagOpen, tagOpen};
        else if (tagClose)
            return Token{TokenType::TagClose, tagClose};
        else if (tagKey)
            return Token{TokenType::TagKey, tagKey};
        else if (tagData)
            return Token{TokenType::TagData, tagData};
        else if (plyNum)
            return Token{TokenType::PlyNum, plyNum};
        else if (placeMove)
            return Token{TokenType::PlaceMove, placeMove};
        else if (moveMove)
            return Token{TokenType::MoveMove, moveMove};
        else if (gameResult)
            return Token{TokenType::GameResult, gameResult};
        else if (comment)
            return Token{TokenType::Comment, comment};
    }
    return std::nullopt;
}

int main()
{
    const std::string tag = "[Site \"PlayTak.com\"]";
    const std::string ptn = "[Site \"playtak.com\"] [Date \"2016.03.15\"] [Time \"00:34:45\"] [Player1 \"Anon\"] [Player2 \"Anon\"] [Result \"0-R\"] [Size \"4\"] 1. b1 c4 2. c1 b3 3. b2 b1+ 4. c2 Sc3 5. a1 c3- 6. a3 b3< 7. d3 b3 8. c4< b3+ 9. b1 2c2- 10. Sb3 c2 11. b3- c3 12. 3b2> b3 13. 2c2+ 3c1+ 14. d4 b2 15. Sa4 d2 16. a4> 4c2-";
    std::size_t startIndex = 0;
    std::size_t len = 1;
    std::vector<Token> items;
    while (startIndex != std::string::npos)
    {
        const auto s = std::string_view(ptn.c_str() + startIndex, len);
        std::cout << "Testing substring " << s << std::endl;
        auto item = lexer(s);
        if (item)
        {
            startIndex += len;
            len = 1;
            items.push_back(*item);
        }
        else
            ++len;

        if (len + startIndex > ptn.size())
            break;
    }
    std::cout << "Tokens: \n";
    for (const auto& item : items)
        std::cout << item.mValue << std::endl;

}