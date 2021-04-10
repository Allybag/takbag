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
    std::ifstream fileStream("games/ShortDump.ptn");
    // std::ifstream fileStream("games/TiltakVsTakoSize6.ptn");

    std::string line;
    while (getline(fileStream, line))
    {
        auto items = lexer.tokenise(line);
        parser.accept(items);
    }
    parser.flush();
    for (const auto& node : parser.getNodes())
        std::cout << node << std::endl;

}
