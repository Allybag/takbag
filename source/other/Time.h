#pragma once

#include <cstddef>
#include <sys/time.h>

static constexpr int64_t micsInSecond = 1000000LL;

inline int64_t timeInMics()
{
    struct timeval currentTime;
    gettimeofday(&currentTime, nullptr);
    return currentTime.tv_sec * micsInSecond + currentTime.tv_usec;
}
