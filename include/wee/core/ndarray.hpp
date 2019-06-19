#pragma once

#include <numeric>
#include <valarray>
#include <array>

namespace wee {
/**
 * https://eli.thegreenplace.net/2015/memory-layout-of-multi-dimensional-arrays/
 */
template <typename Iterator_, typename Iterator>
constexpr size_t linearize(Iterator_ a, Iterator_ b, Iterator dim) {
    if(a == b) {
        return *a;
    } else {
        return (*a) + (*dim) * linearize(std::prev(a), b, std::prev(dim));
    }
}

template <typename T>
size_t linearize(const std::valarray<int>& coord, const std::valarray<T>& dim) {
    return linearize(std::end(coord) - 1, std::begin(coord), std::end(dim) - 1);
    //return std::inner_product(std::begin(dim), std::end(dim), std::begin(coord), 0); 
}
/**
 * implementation based on:
 * https://math.stackexchange.com/questions/2008367/how-to-convert-an-index-into-n-coordinates
 */
template <typename T>
std::valarray<T> delinearize(const size_t k_, const std::valarray<T>& shape) {

    size_t k = k_;
    size_t c = std::accumulate(std::begin(shape), std::end(shape), 1, std::multiplies<size_t>());

    size_t n = shape.size();
    std::valarray<T> res(n);
    for(auto i: wee::range(n)) {
        c /= shape[i];
        auto j  = k / c;
        k -= j * c;
        res[i] = static_cast<T>(j);
    }
    return res;
}
/*
template<typename T>
constexpr T array_product(T x) { return x; }

template<typename T, typename... Ts>
constexpr T array_product(T x, Ts... xs) { return x * array_product(xs...); }
*/

}
