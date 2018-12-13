#include <stack>
#include <tuple>
#include <cmath>
#include <map>
#include <core/range.hpp>
#include <core/random.hpp>
#include <prettyprint.hpp>
#include <type_traits>
#include <chrono>
#include <core/logstream.hpp>
#include <sstream>

using wee::range;

#ifdef _WIN32
#define popcount(x) __popcnt(x)
#else
#define popcount(x) __builtin_popcount(x)
#endif

template<typename T>
constexpr T array_product(T x) { return x; }

template<typename T, typename... Ts>
constexpr T array_product(T x, Ts... xs) { return x * array_product(xs...); }

template <typename Iterator>
constexpr size_t linearize(Iterator first, Iterator last, Iterator first_dim) {
    if(first == last) {
        return *first;
    } else {
        return (*first) + (*first_dim) * linearize(std::next(first), last, std::next(first_dim));
    }
}

#define WEE_DEFAULT_COPY_AND_ASSIGN(T) \
    T(const T&) = default; \
    T& operator = (const T&) noexcept = default; \
    T(T&&) = default; \
    T& operator = (T&&) noexcept = default;
    

template <typename T, size_t Rank>
class tensor {
public:
    using array_type        = std::valarray<T>;
    using index_type        = std::valarray<size_t>;//std::array<size_t, Rank>;
    using value_type        = typename array_type::value_type;      // T
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
    : _data(data) 
    , _shape(shape)
    {
    }

    WEE_DEFAULT_COPY_AND_ASSIGN(tensor);
    virtual ~tensor() = default;

    template <typename... Ts>
    constexpr reference at(Ts... ts) const {
        std::array<size_t, sizeof...(Ts)> ix = { static_cast<size_t>(ts)... };
        //return _data[linearize(std::begin(ix), std::end(ix) - 1, std::begin(_shape))];
        return this->operator[] (ix);
    }

    constexpr reference operator [] (const index_type& ix) {
        //return _data[linearize(std::begin(ix), std::end(ix) - 1, std::begin(_shape))];
        return const_cast<T&>(static_cast<const tensor*>(this)->operator [] (ix));

    }
    constexpr const_reference operator [] (const index_type& ix) const {
        return _data[linearize(std::begin(ix), std::end(ix) - 1, std::begin(_shape))];
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

struct int2 {
    int x, y;
};

void wfc(const int* in_map, const int2& in_size) {
    /**
     * get all unique tiles + their frequencies
     */
    typedef int64_t bitmask_t;
    struct tile { bitmask_t sides[4]; };
    int n = in_size.x * in_size.y;
    std::vector<tile> tiles;
    std::array<int, 256> tile_lookup; // << why 256???
    for(int i=0; i < n; i++) {
        int t = in_map[i];
        if(tile_lookup[t] < 0) {
            tile_lookup[t] = static_cast<int>(tiles.size());
            tiles.push_back(tile{});
        }
    }

}


template <typename T>
tensor<T, 1> arange(T t) {
    tensor<T, 1> res(t);
    std::iota(std::begin(res), std::end(res), 0);
    return res;
}
int main(int, char**) {
    //[[maybe_unused]] tensor<float, 3> t(3, 3, 3);
    auto t = arange(3 * 3 * 3).reshape(3, 3, 3);

    auto va = t.values();
    DEBUG_VALUE_OF(va);
    auto ar = t.slice(1, { 3 }, {3 * 3});

    // iterate through z-dimension at [1, 0]
    for(auto it = std::begin(ar); it != std::end(ar); it++) {
        DEBUG_VALUE_OF(*it);
    }

    return 0;
}
