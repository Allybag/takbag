#include <iostream>

#include "cmdLine.h"
#include "log/Logger.h"
#include "other/ArgParse.h"
#include "playtak.h"
#include "tei.h"

int main(int argc, const char* argv[])
{
    auto options = parseArgs(argc, argv);
    if (options.contains("logToStdOut"))
    {
        // TODO: We want a nicer way to do this
        if (options.at("logToStdOut") == "false")
            rootLogger.setLogToStdOut(false);
        else if (options.at("logToStdOut") == "true")
            rootLogger.setLogToStdOut(true);
    }

    if (options.contains("tei"))
        tei(options);
    else if (options.contains("playtak"))
        playtak(options);
    else if (options.contains("cli"))
        playCommandLine(options);
    else
        tei(options); // Make this the default just so people can run with TEI without passing any arguments

    // for (const auto [key, value] : options)
    // std::cout << "Option: " << key << " , Value: " << value << std::endl;

    return 0;
}
