cmake_minimum_required(VERSION 3.13)
project(takbag)

# Use C++20, with all warnings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Weverything")
set(CMAKE_CXX_FLAGS_RELEASE "-flto -O3 -DNDEBUG") # TODO: Is lto actually working?

# Except for
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic") # Who cares about C++98?
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-padded") # Pragmas differ, padding doesn't matter
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-conversion -Wno-sign-compare -Wno-shorten-64-to-32") # int is an int
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-implicit-int-conversion -Wno-missing-prototypes") # Prototype schmototype
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ctad-maybe-unsupported") # It may well be supported
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors -Wno-exit-time-destructors") # Sometimes I want statics

include_directories(/external)
include_directories(source)
add_subdirectory(source)
add_subdirectory(test)
