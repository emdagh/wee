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

size_t linearize(const std::valarray<int>& coord, const std::valarray<size_t>& dim) {
    return linearize(std::end(coord) - 1, std::begin(coord), std::end(dim) - 1);
}
/**
 * implementation based on:
 * https://math.stackexchange.com/questions/2008367/how-to-convert-an-index-into-n-coordinates
 */
template <typename T>
std::valarray<T> delinearize(const size_t k_, const std::valarray<size_t>& shape) {

    size_t k = k_;
    size_t c = std::accumulate(std::begin(shape), 
        std::end(shape), 
        1, 
        std::multiplies<size_t>()
    );

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
template<typename T>
constexpr T array_product(T x) { return x; }

template<typename T, typename... Ts>
constexpr T array_product(T x, Ts... xs) { return x * array_product(xs...); }


template <typename T>
constexpr std::valarray<T> to_cartesian(const std::valarray<T>& in) {
    std::valarray<T> copy(in);
    return std::reverse(std::begin(copy), std::end(copy));
}



template <typename T, size_t Rank>
class tensor {
public:
    using array_type        = std::valarray<T>;
    using index_type        = std::valarray<size_t>;
    using value_type        = typename array_type::value_type;
    using reference         = T&;
    using const_reference   = const T&;

    using iterator = T*;
    using const_iterator = const T*;
    
    constexpr iterator       begin()        { return std::begin(_data);  }
    constexpr const_iterator begin()  const { return std::begin(_data);  }
    constexpr const_iterator cbegin() const { return std::cbegin(_data); }
    constexpr iterator       end()          { return std::end(_data);    }
    constexpr const_iterator end()    const { return std::end(_data);    }
    constexpr const_iterator cend()   const { return std::cend(_data);   }

    tensor() { }

    template <typename... Ts>
    tensor(Ts... args) 
    : _shape{static_cast<size_t>(args)...} 
    {
        static_assert(sizeof...(Ts) == Rank);
        
        auto n = array_product(args...);//std::accumulate(std::begin(_shape), std::end(_shape), 1, std::multiplies<int>());
        
        _data.resize(n);
    }

    tensor(const array_type& data, const index_type& shape) 
    : _shape(shape)
    , _data(data) 
    {
    }

    WEE_DEFAULT_COPY_AND_ASSIGN(tensor);
    virtual ~tensor() = default;

    template <typename... Ts>
    constexpr reference at(Ts... ts) const {
        std::array<size_t, sizeof...(Ts)> ix = { static_cast<size_t>(ts)... };
        return this->operator[] (ix);
    }

    constexpr reference operator [] (const index_type& ix) {
        return const_cast<T&>(static_cast<const tensor*>(this)->operator [] (ix));

    }
    constexpr const_reference operator [] (const index_type& ix) const {
        return _data[linearize(std::end(ix) - 1, std::begin(ix), std::end(_shape)-1)];
    }

    array_type slice(size_t s, const index_type& d, const index_type& i) {
        return _data[std::gslice(s, d, i)];
    }

    const index_type& shape() const {
        return _shape;
    }
    
    template <size_t S = 1>
    constexpr std::array<int, 2 * S * Rank> stencil(size_t ix) const {
        auto stencil_indices = std::array<int, 2 * S * Rank> {};
        auto get_neighbor = [this] (size_t ix) {
            return (ix >= 0 && ix < _data.size()) ? static_cast<int>(ix) : -1;
        };
        size_t offset_dim = 1;
        for(size_t d=0; d < Rank; d++) {
            stencil_indices[d * Rank + 0] = get_neighbor(ix - offset_dim); // left
            stencil_indices[d * Rank + 1] = get_neighbor(ix + offset_dim); // right
            offset_dim *= _shape[d];
        }
        return stencil_indices;
    }

    array_type& values() {
        return const_cast<array_type&>(static_cast<const tensor*>(this)->values());
    }
    const array_type& values() const {
        return _data;
    }

    template <typename... Ts>
    tensor& reshape(Ts... args) {
        assert(array_product(static_cast<size_t>(args)...) == _data.size());
        _shape = { static_cast<size_t>(args)... };
        return *this;
    }

private:
    index_type _shape;
    array_type _data;
};
}
