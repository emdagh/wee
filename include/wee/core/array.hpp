#pragma once

#include <array>
#include <core/algorithm.hpp>


namespace wee {

template <typename T, size_t N, T Value, typename... Ts>
std::array<T, N> make_array(Ts... ts) {
    if constexpr(sizeof...(Ts) < N) {
        return make_array<T, N, Value>(ts..., Value);
    }
    return std::array<T,N> { ts... };
}

    template <typename T, size_t N1, size_t... Isa, size_t N2, size_t... Isb>
    constexpr std::array<T, N1 + N2> array_join_impl (
        const std::array<T, N1>& a,
        const std::array<T, N2>& b,
        std::index_sequence<Isa...>,
        std::index_sequence<Isb...>)
    {
        return { 
            a[Isa]..., 
            b[Isb]... 
        };
    }

    template <typename T, size_t N1, size_t N2>
    constexpr std::array<T, N1 + N2> array_join(
        const std::array<T, N1>& a,
        const std::array<T, N2>& b
    ) {
        return array_concat_impl(a, b, std::make_index_sequence<N1>(), std::make_index_sequence<N2>());
    }



    template <typename T, size_t... Is>
    constexpr auto array_product_impl(const std::array<T, sizeof...(Is)>& t, std::index_sequence<Is...>) {
        return (1 * ... * (t[Is]));
    }

    template <typename T, size_t N>
    constexpr auto array_product(const std::array<T, N>& t) {
        return array_product_impl(t, std::make_index_sequence<N>{});
    }
    template <typename T, size_t... Is>
    constexpr auto array_sum_impl(const std::array<T, sizeof...(Is)>& t, std::index_sequence<Is...>) {
        return (0 + ... + (t[Is]));
    }

    template <typename T, size_t N>
    constexpr auto array_sum(const std::array<T, N>& t) {
        return array_sum_impl(t, std::make_index_sequence<N>{});
    }

    template <typename T, size_t... Is>
    constexpr auto add_impl(
        const std::array<T, sizeof...(Is)>& a, 
        const std::array<T, sizeof...(Is)>& b, 
        std::index_sequence<Is...>) 
    {
        return std::array<T, sizeof...(Is)> {
            (a[Is] + b[Is])...
        };
    }

    template <typename T, size_t N>
    constexpr auto add(const std::array<T, N>& a, const std::array<T, N>& b) {
        return add_impl(a, b, std::make_index_sequence<N>());
    }
}

template <typename T, size_t N>
std::array<T, N> operator + (const std::array<T, N>& a, const std::array<T, N>& b) {
    std::array<T, N> res;
    std::transform(a.begin(), a.end(), b.begin(), res.begin(), std::plus<T>());
    return res;
    //return add(a, b);
}
template <typename T, size_t N>
std::array<T, N> operator - (const std::array<T, N>& a, const std::array<T, N>& b) {
    std::array<T, N> res;
    std::transform(a.begin(), a.end(), b.begin(), res.begin(), std::minus<T>());
    return res;
}

template<typename T, size_t N>
std::array<T, N> operator % (const std::array<T, N>& a, const std::array<T, N>& b) {
    std::array<T, N> res;
    std::transform(a.begin(), a.end(), b.begin(), res.begin(), std::modulus<T>());
    return res;
}
template<typename T, size_t N>
std::array<T, N> operator * (const std::array<T, N>& a, const std::array<T, N>& b) {
    std::array<T, N> res;
    std::transform(a.begin(), a.end(), b.begin(), res.begin(), std::multiplies<T>());
    return res;
}
