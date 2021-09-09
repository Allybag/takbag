#pragma once

// Taken from an answer by https://stackoverflow.com/users/567292/ecatmur to
// https://stackoverflow.com/questions/11526526/how-to-combine-static-assert-with-sizeof-and-stringify
template <typename ToCheck, std::size_t ExpectedSize, std::size_t RealSize = sizeof(ToCheck)>
struct SizeChecker
{
    static_assert(ExpectedSize == RealSize, "Size is off!");
    void operator()() { }
};
