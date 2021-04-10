#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "boost/ut.hpp"
#pragma clang diagnostic pop

#include <fstream>

#include "Lexer.h"
#include "PtnParser.h"

int main()
{
    Lexer lexer;
    Parser parser;
    std::ifstream fileStream("/Users/ally/clones/takbag/games/ShortDump.ptn");

    std::string line;
    while (getline(fileStream, line))
    {
        auto items = lexer.tokenise(line);
        parser.accept(items);
    }
    parser.flush();
    std::cout << "Tokens: \n";
    for (const auto& node : parser.getNodes())
        std::cout << node << std::endl;

}
