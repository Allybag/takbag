#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include <fstream>

#include "Lexer.h"

int main()
{
    Lexer lexer;
    std::ifstream fileStream("/Users/ally/clones/takbag/games/LongStackyGame.ptn");

    std::string line;
    std::vector<Token> tokens;
    while (getline(fileStream, line))
    {
        auto items = lexer.tokenise(line);
        for (const auto& item : items)
        {
            tokens.push_back(item);
        }
    }
    std::cout << "Tokens: \n";
    for (const auto& token : tokens)
        std::cout << token << std::endl;

}
