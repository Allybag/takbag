#pragma once

#include "other/Time.h"

#include <iostream>
#include <cstdint>
#include <string>

#define AlwaysInline __attribute__((always_inline))

// Taken from Chandler Carruth (https://youtu.be/nXaxk27zwlk?t=2441)
// and Google Benchmark (https://github.com/google/benchmark)
template <typename T>
inline AlwaysInline void DoNotOptimizeAway(const T& value)
{
    asm volatile("" : : "r,m"(value) : "memory");
}

#define runBenchmark(func) benchmark(#func, func);

template <typename LambdaT>
void benchmark(const std::string& name, LambdaT benchFunction, std::size_t runCount = 10'000'000)
{
    auto before = timeInMics();
    for (std::size_t i = 0; i < runCount; ++i)
    {
        DoNotOptimizeAway(benchFunction());
    }

    auto after = timeInMics();
    auto duration = after - before;
    auto averageDuration = duration * 1000 / runCount;

    std::cout << name << " run took an average of " << averageDuration << " nanos" << std::endl;
}