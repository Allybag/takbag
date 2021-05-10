cmake_minimum_required(VERSION 3.13)
project(takbag)

# Use C++20, with all warnings
set(CMAKE_CXX_STANDARD 20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything") # Enable everything all warnings, except for

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic") # Who cares about C++98?
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-padded") # Pragmas differ, padding doesn't matter
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-conversion -Wno-sign-compare -Wno-shorten-64-to-32") # int is an int
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-implicit-int-conversion -Wno-missing-prototypes") # Prototype schmototype
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ctad-maybe-unsupported") # It may well be supported
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors -Wno-exit-time-destructors") # Sometimes I want statics
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    message("This project has never been tested using the Intel Compiler")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message("This project has never been tested using Microsoft Visual Studio")
endif()

if (APPLE)
    # Nothing to do, this is my default
elseif (UNIX AND NOT APPLE) # Linux
    # Not actually Linux specific, just my Fedora server only has 1 gig of RAM
    set (LOW_MEMORY TRUE)
elseif (WIN32)
    message("This project has never been tested on Windows")
endif()

# TODO: Look into LTO/IPO, and adding some flags like -march=native -fno-rtti -Ofast and so on
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")

if (LOW_MEMORY)
    set (CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DLOW_MEMORY_COMPILE")
endif()

include_directories(/external)
include_directories(source)
add_subdirectory(source)
add_subdirectory(test)
