#pragma once

#include <bitset>

namespace wee {
    template <typename T>
    constexpr int ctz(const T& t) {
        throw not_implemented("unknown type for ctz");
    }

    template <>
    constexpr int ctz<unsigned int>(const unsigned int& i) {
        return __builtin_ctz(i);
    }
    template <>
    constexpr int ctz<unsigned long>(const unsigned long& l) {
        return __builtin_ctzl(l);
    }
    template <>
    constexpr int ctz<unsigned long long>(const unsigned long long& l) {
        return __builtin_ctzll(l);
    }

    template <typename T, size_t N = sizeof(T)>
    constexpr int popcount(const T&) {
        throw not_implemented();
    }

    template <>
    constexpr int popcount<unsigned int>(const unsigned int& i) {
        return __builtin_popcount(i);
    }
    template <>
    constexpr int popcount<unsigned long>(const unsigned long& l) {
        return __builtin_popcountl(l);
    }
    template <>
    constexpr int popcount<unsigned long long>(const unsigned long long& l) {
        return __builtin_popcountll(l);
    }


    /*template <>
    constexpr int popcount<std::bitset<64>>(const std::bitset<64>& b) {
        return b.count();
    }*/
}


