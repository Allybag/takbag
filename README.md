# Takbag
Implementation of Tak (https://cheapass.com/tak/)

# Compiling
takbag uses C++20, so requires a modern C++ compiler  
Only Clang 12 has been tested.  
takbag uses CMake, but should be easy to compile without.  

# External
takbag has no external dependencies.  
All files in external are copied from external repositories and a frozen copy is included with takbag

external/boost/LICENSE_1_0.txt (https://www.boost.org/users/license.html)  
external/boost/ut.hpp (https://github.com/boost-ext/ut)  
external/ctre.hpp (https://github.com/hanickadot/compile-time-regular-expressions)  

# Goals
Support TEI protocol (github.com/MortenLohne/racetrack/) to play against other bots  
Support Playtak protocol (playtak.com) which is sadly not PTN, and needs some web socket work  
Support TPS protocol to easily give customised positions to the engine (might be required for TEI)  
A simple search function using negamax (which would work as a nice Tinue solver)  
A simple search function using MonteCarlo  
A simple evaluation based on flat count  
A simple evaluation based on road potential  
Be able to easily combine different search and evaluation functions to make a strong/interesting opponent
