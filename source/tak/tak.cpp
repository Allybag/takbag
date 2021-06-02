#include <iostream>

#include "other/ArgParse.h"
#include "tei.h"
#include "cmdLine.h"
#include "playtak.h"
#include "log/Logger.h"

int main(int argc, const char* argv[])
{
    auto options = parseArgs(argc, argv);
    if (options.contains("logToStdOut"))
    {
        if (options.at("logToStdOut") == "false")
            rootLogger.setLogToStdOut(false);
    }

    if (options.contains("tei"))
        tei(options);
    else if (options.contains("playtak"))
        playtak(options);
    else
        tei(options);

    // for (const auto [key, value] : options)
        // std::cout << "Option: " << key << " , Value: " << value << std::endl;

    return 0;
}


