#pragma once

#include <wee.hpp>
#include <bitset>
#include <array>

namespace wee {
#if 1
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
#else
    template <typename T>
    constexpr int popcount(T x) {
        int r = 0;
        for(; x; x &= x-1)
            ++r;
        return r;
}

#endif

    /*template <>
    constexpr int popcount<std::bitset<64>>(const std::bitset<64>& b) {
        return b.count();
    }*/

    using byte = int8_t;

    template <typename T>
    std::array<byte, sizeof(T)> to_bytes(const T& t) {
        std::array<byte, sizeof(T)> res;
        const byte* first = reinterpret_cast<const int8_t*>(std::addressof(t));
        const byte* last  = first + sizeof(T);
        std::copy(first, last, res.begin());
        return res;
    }

    template< typename T >
    T& from_bytes( const std::array< byte, sizeof(T) >& bytes, T& object ) {
        // http://en.cppreference.com/w/cpp/types/is_trivially_copyable
        static_assert( std::is_trivially_copyable<T>::value, "not a TriviallyCopyable type" ) ;
        byte* begin_object = reinterpret_cast< byte* >( std::addressof(object) ) ;
        std::copy( std::begin(bytes), std::end(bytes), begin_object ) ;
        return object ;
    }

template <typename T>
void pop_bits(T& d_bits, const T& mask) {
    d_bits &= ~mask;
}

template <typename T>
void push_bits(T& d_bits, const T& mask) {
    d_bits |= mask;
}

}


