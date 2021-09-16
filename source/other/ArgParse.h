#pragma once

#include "other/StringOps.h"

#include <cassert>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

using OptionMap = std::unordered_map<std::string, std::string>;

void flush(OptionMap& options, std::string& optionName, std::vector<std::string>& optionData)
{
    if (optionName.empty())
    {
        assert(optionData.empty());
        return;
    }

    if (!optionData.empty())
        options[optionName] = join(optionData, ' ');
    else
        options[optionName] = "true";

    optionName.clear();
    optionData.clear();
}

OptionMap parseArgs(int argc, const char* argv[])
{
    OptionMap options;
    std::string optionName;
    std::vector<std::string> optionData;

    for (std::size_t index = 1; index < argc; ++index) // We start at one as argv[0] is binary name
    {
        auto currentOption = std::string(argv[index]);

        if (currentOption.front() == '-')
        {
            flush(options, optionName, optionData);
            optionName = currentOption.substr(1);
        }
        else if (currentOption.front() == '+')
        {
            flush(options, optionName, optionData);
            options[currentOption.substr(1)] = "false";
        }
        else
        {
            optionData.push_back(currentOption);
        }
    }

    flush(options, optionName, optionData);
    return options;
}
