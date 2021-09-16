#pragma once

#include <sstream>
#include <string>
#include <vector>

inline std::string join(const std::vector<std::string>& tokens, char delimiter, std::size_t startIndex = 0,
                        std::size_t endIndex = 0)
{
    if (endIndex == 0)
        endIndex = tokens.size() - 1;
    std::string s;
    for (std::size_t index = startIndex; index <= endIndex; ++index)
    {
        s.append(tokens[index]);
        if (index != endIndex) // We don't want a trailing delimiter
            s.push_back(delimiter);
    }

    return s;
}

inline std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}
