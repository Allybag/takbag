# Takbag
Implementation of Tak (https://cheapass.com/tak/)

# Compiling
takbag uses C++20, so requires a modern C++ compiler  
Only Clang 12 has been tested.  
takbag uses CMake, but should be easy to compile without.  

# Structure
We have an implementation of the rules of tak, with the ability to generate moves and determine the result of the game in source/tak  
We have an engine to play against in source/engine  
We have a client to connect and speak to Playtak.com in source/playtak  
We have a very simple TCP client in source/tcp  
We have a very simple single threaded, std::ostream like logger in source/log  
We have a toy parser in source/ptn that should be able to read files containing one or more PTN games in source/ptn  

# External
takbag has no external dependencies.  
All files in external are copied from external repositories and a frozen copy is included with takbag

external/boost/LICENSE_1_0.txt (https://www.boost.org/users/license.html)  
external/boost/ut.hpp (https://github.com/boost-ext/ut)  
external/ctre.hpp (https://github.com/hanickadot/compile-time-regular-expressions)  

# Goals
A simple evaluation based on road potential  
Be able to easily combine different search and evaluation functions to make a strong/interesting opponent
~~Support TEI protocol (github.com/MortenLohne/racetrack/) to play against other bots~~  
~~Support Playtak protocol (playtak.com) which is sadly not PTN, and needs some web socket work~~  
~~A simple search function using negamax (which would work as a nice Tinue solver)~~  
~~A simple search function using MonteCarlo~~  
~~A simple evaluation based on flat count~~  
