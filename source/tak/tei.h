#pragma once

#include "Game.h"
#include "engine/Engine.h"
#include "other/StringOps.h"

#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

Game parseTeiPosition(std::vector<std::string> words, std::size_t size)
{
    assert(words.front() == "position");
    assert(words[1] == "startpos"); // TODO: Could also be a TPS string
    assert(words[2] == "moves");

    auto game = Game(size);
    for (std::size_t index = 3; index < words.size(); ++index)
        game.play(words[index]);

    return game;
}

void tei(const OptionMap&)
{
    Logger logger("tei");
    // We probably want to deal with the "tei" string before here
    std::string input;
    std::cin >> input;

    assert(input == "tei");

    std::string name = "BreadBot";
    std::string author = "Ally";

    std::cout << "id name " << name << std::endl;
    std::cout << "id author " << author << std::endl;
    std::cout << "teiok" << std::endl;

    std::size_t size = 5;
    Game game(size);

    while (getline(std::cin, input))
    {
        logger << LogLevel::Info << "Received " << input << Flush;
        auto words = split(input, ' ');
        if (words.empty())
            continue;
        auto command = words.front();
        if (command == "quit")
            break;
        else if (command == "isready")
            std::cout << "readyok" << std::endl;
        else if (command == "teinewgame")
        {
            assert(words.size() == 2);
            size = std::stoi(words[1]);
            assert(size >= 3 && size <= 8);
        }
        else if (command == "position")
        {
            game = parseTeiPosition(words, size);
        }
        else if (command == "go")
        {
            auto colour = game.getPosition().getPlayer();
            auto millisRemaining = colour == Player::White ? std::stoi(words[2]) : std::stoi(words[4]);
            double thinkingTime = (millisRemaining / 1000.0) / 10.0;
            std::cout << "info score cp 1 time 1 pv a1" << std::endl; // TODO: This is obviously nonsense
            Engine engine;
            std::cout << "bestmove " << engine.chooseMove(game.getPosition(), thinkingTime) << std::endl;
            continue;
        }
        else
        {
            std::cout << "Unrecognised input: " << input << std::endl;
            break;
        }
    }
}
