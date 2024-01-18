#pragma once

#include <cstdint>
#include <functional>

inline void hash_combine_internal(std::size_t&) { }

template <typename T, typename... Rest>
inline void hash_combine_internal(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hash_combine_internal(seed, rest...);
}

template <typename... Rest>
inline std::size_t hash_combine(Rest... rest) {
    std::size_t hash{0};
    hash_combine_internal(hash, rest...);
    return hash;
}
