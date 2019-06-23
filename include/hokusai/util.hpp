#pragma once

template <typename T, T First, typename F, T... Is>
constexpr auto static_for_impl(F&& f, std::integer_sequence<T, Is...>) {
    return (std::forward<F>(f)(std::integral_constant<T, First + Is> {}), ...);
}


template <typename T, T First, T Last, typename F>
constexpr auto static_for(F&& f) {
    return static_for_impl<T, First>(std::forward<F>(f), std::make_integer_sequence<T, Last - First>());
}

template <typename T>
constexpr T to_bitmask(T idx) { return static_cast<T>(1) << idx; }


template <typename T>
constexpr T to_index(T t) { return ctz(t); }

template <typename T, typename Iter>
void avail(T t, Iter it) {
    T tmp = t;
    size_t i=0, n=popcount(tmp);
    while (i++ < n) { 
        *it++ = to_index(tmp);
        auto lb = tmp & -tmp;
        tmp ^= lb;
    }
}

template <typename T>
constexpr float entropy_of(T t) { return 1.0f - 1.0f / static_cast<float>(popcount(t)); }

template <typename T>
void trace(T* ptr, size_t start, size_t n, size_t stride, T val) {
    for(size_t i=0; i < n; i+=stride)  ptr[start + i] = val; 
}
template <size_t N> //, const size_t M = N * (N<<1)>
constexpr std::array<ptrdiff_t, N * (N<<1)>  make_direction_index() {

    const size_t M = N * (N << 1);
    std::array<ptrdiff_t, M> res = { 0 };
    trace<ptrdiff_t>(&res[0], 0,     M >> 1, N + 1,  1);
    trace<ptrdiff_t>(&res[0], N * N, M >> 1, N + 1, -1);
    return res;
}


template <size_t N>
using direction_index = std::array<ptrdiff_t, N * (N << 1)>;//= build();

template <size_t N>
std::array<ptrdiff_t, N * (N << 1)> topology<N>::sides = make_direction_index<N>();

