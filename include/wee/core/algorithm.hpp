#pragma once

#include <algorithm>
#include <array>

namespace wee {
    template <typename T, size_t N, size_t... Is>
    constexpr static size_t inner_product_impl(
        const std::array<T,N>& a, 
        const std::array<T,N>& b,
        T start,
        std::index_sequence<Is...>
    ) {
        return (start + ... + (a[Is] * b[Is]));
    }

    template <typename T, size_t N>
    constexpr static size_t inner_product(const std::array<T, N>& a, const std::array<T, N>& b, T start = T {}) {
        return inner_product_impl(a, b, start, std::make_index_sequence<N>());
    }
}
