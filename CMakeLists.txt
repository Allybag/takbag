cmake_minimum_required(VERSION 3.13)
project(takbag)

# Use C++17, with all warnings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Weverything")

# Except for
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic") # Who cares about C++98?
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-padded") # Pragmas differ, padding doesn't matter
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-conversion -Wno-sign-compare -Wno-shorten-64-to-32") # An int is an int
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-prototypes") # Prototype schmototype

add_executable(tak tak.cpp)
