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
    }
    return stream;
}

struct Token {
    TokenType mType;
    std::string_view mValue;
};

std::ostream& operator<<(std::ostream& stream, const Token& token)
{
    stream << "Token of type " << token.mType << " containing " << token.mValue;
    return stream;
}

// Oh Boy
static constexpr auto pattern = ctll::fixed_string{"\\s*(\\[)|(\\])|([A-Za-z0-9_]+\\s)|\"(.*)\"|\\s*([1-9][0-9]*)\\.\\s|\\s*([CSF]?[a-h][1-8]['!?]*)\\s|\\s*([1-8]?[a-h][1-8][+-<>][1-8]*[CSF]?\\*?['!?]*)\\s|\\s*([RF10](?:/2-1/2)|(?:-[RF10]))|\\s*\\{(.*?)\\}"};
static constexpr auto resultPattern = ctll::fixed_string{"\\s*([RF10](?:/2-1/2)|(?:-[RF10]))"};

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
    const std::string commentedPtn = "[Event \"{bumdee bum}\"] [Site \"?\"] [Date \"2021.02.05\"] [Round \"0\"] [Player1 \"tiltak\"] [Player2 \"taktician\"] [Result \"1-0\"] [Size \"6\"] 1. c5 b1 2. e1 d5 3. d1 {+0.14/0 2.96s} c1 {+0.80/0 0.81s \"Boy\" } 4. d2 {+0.06/0 9.83s} b5 {+0.80/0 0.80s} 5. d3 {+0.04/0 4.54s} b4 {0.00/0 0.88s} 6. e5 {+0.19/0 4.64s} d4 {-0.50/0 1.23s} 7. e4 {+0.24/0 0.88s} e3 {-4.00/0 6.41s} 8. e2 {+0.23/0 1.20s} b3 {+0.50/0 1.28s} 9. e6 {+0.49/0 0.87s} d5> {-0.60/0 1.03s} 10. Cd5 {+0.59/0 3.99s} Cf4 {-2.70/0 1.61s} 11. e4< {+0.77/0 1.43s} e3< {-5.30/0 0.62s} 12. e3 {+0.78/0 0.93s} c3 {-7.10/0 1.07s} 13. d6 {+0.79/0 4.16s} 2d3+ {-12.20/0 1.48s} 14. d5- {+0.82/0 3.24s} c2 {-8.60/0 5.54s} 15. b1> {+0.88/0 0.84s} Sd3 {-13.70/0 0.41s} 16. d5 {+0.93/0 0.97s} c5> {-13.60/0 5.06s} 17. b1 {+0.93/0 4.47s} c2- {-14.40/0 0.96s} 18. e4 {+0.94/0 2.32s} 3c1>12 {-12.60/0 1.36s} 19. b2 {+0.93/0 4.81s} c2 {-11.20/0 2.07s} 20. b6 {+0.90/0 2.02s} f4< {-8.30/0 2.07s} 21. c5 {+0.89/0 2.17s} c1 {-8.00/0 2.85s} 22. e2- {+0.88/0 1.56s} 2d1> {-10.10/0 2.01s} 23. c6 {+0.83/0 0.80s} Sc4 {-8.60/0 3.15s} 24. a6 {+0.92/0 0.78s} b5+ {-9.00/0 2.37s} 25. Sb5 {+0.92/0 1.70s} 6e1+33 {-9.80/0 3.41s} 26. f6 {+0.93/0 2.22s} e1 {-7.00/0 2.11s} 27. d2> {+0.92/0 0.53s} e1+ {-8.80/0 2.13s} 28. d2 {+0.92/0 2.30s} 2e2< {-7.20/0 2.83s} 29. b2> {+0.88/0 0.91s} d1 {-8.70/0 2.61s} 30. c6< {+0.83/0 0.88s} Sc6 {-3.70/0 2.19s} 31. b2 {+0.80/0 0.76s} a3 {-7.10/0 2.25s} 32. f2 {+0.73/0 0.69s} e1 {-6.70/0 2.08s} 33. f2< {+0.82/0 0.52s} e1+ {-8.20/0 0.17s} 34. Se1 {+0.79/0 0.52s} 5e2<32 {-7.00/0 1.98s} 35. Se2 {+0.84/0 0.45s} 6d2<33 {-7.10/0 0.19s} 36. d2 {+0.88/0 0.59s} 3c2> {-2.10/0 0.07s} 37. e2< {+0.98/0 0.35s} 4b2- {-20.00/0 1.93s} 38. 4d2<112 {+0.97/0 2.24s} e2 {-17.80/0 0.31s} 39. b2- {+0.97/0 1.84s} c1< {-14.50/0 1.33s} 40. b2 {+0.96/0 1.65s} c1 {-17.70/0 0.25s} 41. b2- {+0.96/0 1.36s} c1< {-18.70/0 1.79s} 42. b2 {+0.91/0 1.26s} c1 {-18.30/0 0.27s} 43. b2- {+0.91/0 1.07s} c1< {-14.80/0 0.73s} 44. 2a2> {+0.88/0 0.56s} 6b1> {-20.00/0 0.28s} 45. 2c2- {+0.92/0 0.98s} d1< {-23.30/0 0.36s} 46. c2- {+0.94/0 0.91s} 5b1> {-8052972.48/0 1.71s} 47. 2c2- {+0.97/0 0.89s} c6< {-8052972.48/0 0.00s} 48. 6c1+33 {+0.98/0 0.69s} 2b6< {-8052972.48/0 0.00s} 49. c6 {+1.00/0 0.39s} 1/2-1/2";
#if 0
    const std::string resultString = "1/2-1/2";
    if (auto [match, result] = ctre::match<resultPattern>(resultString); match)
        std::cout << Token{TokenType::GameResult, result} << std::endl;
    return 0;
#endif

    std::size_t startIndex = 0;
    std::size_t len = 1;
    std::vector<Token> items;
    while (startIndex != std::string::npos)
    {
        const auto s = std::string_view(commentedPtn.c_str() + startIndex, len);
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

        if (len + startIndex > commentedPtn.size())
            break;
    }
    std::cout << "Tokens: \n";
    for (const auto& item : items)
        std::cout << item << std::endl;

}