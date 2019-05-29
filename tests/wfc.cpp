#include <prettyprint.hpp>
#include <wee.hpp>
#include <sstream>
#include <numeric>
#include <vector>
#include <core/range.hpp>
#include <core/ndview.hpp>
#include <core/bits.hpp>
#include <core/random.hpp>
#include <core/array.hpp>
#include <core/logstream.hpp>

#include <stack>
#include <unordered_map>

using namespace wee;

/** helper functions */
template <typename T, size_t N, size_t... Is>
constexpr size_t linearize_array_impl(const wee::ndindexer<N>& ix, const T& ary, std::index_sequence<Is...>) {
    return ix.linearize((ary[Is])...);
}

template <typename T, size_t N>
constexpr size_t linearize_array(const wee::ndindexer<N>& ix, const T& ary) {
    return linearize_array_impl<T, N>(ix, ary, std::make_index_sequence<N>()); 
}

template <typename T, T First, typename F, T... Is>
constexpr auto static_for_impl(F&& f, std::integer_sequence<T, Is...>) {
    return (std::forward<F>(f)(std::integral_constant<T, First + Is> {}), ...);
}


template <typename T, T First, T Last, typename F>
constexpr auto static_for(F&& f) {
    return static_for_impl<T, First>(std::forward<F>(f), std::make_integer_sequence<T, Last - First>());
}

template <typename T>
constexpr T to_bitmask(T idx) { return 1 << idx; }


template <typename T>
constexpr T to_index(T t) { return ctz(t); }

template <typename T, typename Iter>
void avail(T t, Iter it) {
    T tmp = t;
    for (auto i : range(popcount(tmp))) {
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

template <size_t N, const size_t M = N * (N<<1)>
constexpr auto make_direction_index() {
    //static const size_t M = N * (N << 1);
    std::array<ptrdiff_t, M> res;
    trace<ptrdiff_t>(&res[0], 0,     M >> 1, N + 1,  1);
    trace<ptrdiff_t>(&res[0], N * N, M >> 1, N + 1, -1);
    return res;
}

template <size_t N>
using direction_index = std::array<size_t, N * (N << 1)>;//= build();


template <size_t N>
struct topology {
    static const int kNumNeighbors = N << 1;
    static std::array<ptrdiff_t, N * (N<<1)> sides;

    typedef std::array<ptrdiff_t, N> value_type;

    value_type _shape { 0 };

    constexpr bool is_valid(const value_type& c) const {
#if 0 // fast and ugly
        int count = 0;
        static_for<size_t, 0, N>([&] (auto i) {
            if(c[i] < 0 || _shape[i] <= c[i]) count++;
        });
        return count==0;
#else
        for (auto i : range(N)) {
            if (c[i] < 0 || _shape[i] <= c[i]) {
                return false;
            }
        }
        return true;
#endif
    }

    auto to_coordinate(size_t i) const { 
        ndindexer<N> ix(_shape);
        return ix.delinearize(i);
    }

    constexpr size_t to_index(const value_type& at) const {
        typedef wee::ndindexer<N> indexer_type;
        indexer_type ix(_shape);
        return linearize_array(ix, at);
    }

    constexpr bool try_move(const value_type& from, const value_type& d, size_t* d_index) const {
        auto to = from + d;
        if (is_valid(to)) {
            return *d_index = to_index(to), true;
        }
        return false;
    }

    constexpr bool try_move(const value_type& from, size_t i, size_t* d_index) const {
        return try_move(from, neighbor(i), d_index);
    }
    constexpr bool try_move(size_t i, size_t j, size_t* d_index) const {
        return try_move(to_coordinate(i), j, d_index);
    }

    template <typename OutputIt>
    void neighbor_copy(size_t i, OutputIt d_first) const {
        auto first = sides.begin() + i * N;
        auto last = first + N;
        std::copy(first, last, d_first);
    }

    value_type neighbor(size_t i) const {
        value_type res;
        neighbor_copy(i, res.begin());
        return res;
    }
};


template <size_t N>
std::array<ptrdiff_t, N * (N << 1)> topology<N>::sides = make_direction_index<N>();


template <typename T, size_t N>
struct adjacency_list {
    typedef typename topology<N>::value_type shape_type;
    
    static const size_t kNumNeighbors = N << 1;

    std::vector<T> _data;

    adjacency_list(size_t n) {
        _data.resize(n * kNumNeighbors);
    }
    /**
     * add an adjacency in a specified direction
     */

    void add(size_t i_a, size_t i_b, size_t d) {
        _data[i_a * N + d] |= to_bitmask(i_b);
    }


    template<typename InputIt>
    void add_example(InputIt first, InputIt last, const topology<N>& topo) {
        InputIt begin = first;
        while(first != last) {
            size_t a = last - first;
            for(auto i: range(kNumNeighbors)) {
                size_t b;
                if(topo.try_move(a, i, &b)) {
                    add(static_cast<size_t>(*(begin + a)), static_cast<size_t>(*(begin + b)), i);
                }
            }
        }
    }


    const T& operator [] (size_t i) const { return _data.at(i); }
};

template <typename T>

struct wave {
    std::vector<T> _data;
    wee::random _rand;

    void pop(size_t i, T t) {
        _data[i] &= ~t;
    }

    bool is_collapsed(T t) const { return popcount(t) == 1; }

    size_t min_entropy_index() {
        size_t ret = 0;
        float min_h = std::numeric_limits<float>::infinity();
        for (T i : _data) {
            if (is_collapsed(i)) 
                continue;

            float h = entropyof(i) - _rand.next<float>(0.0f, 1.0f) / 1000.0f;
            if (h < min_h) {
                min_h = h;
                ret = i;
            }
        }
        return ret;
    }

};

template <typename T>
struct tileset {

    std::vector<T> _data;
    std::unordered_map<T, size_t> _names;
    std::vector<float> _frequency;

    size_t index_for_tile(T t) const { return _data[_names[t]]; }
    void set_frequency(T t, float f) { _frequency[_names[t]] = f; }
};

template <typename T, size_t N>
struct wave_propagator {
    static const size_t kNumNeighbors = N << 1;

    typedef typename topology<N>::value_type coordinate_type;

    topology<N> _topo;


    void propagate(size_t at, const wave<T>& w, const adjacency_list<T, N>& adj) const {
        std::vector<T> open = { at };
        while(!open.empty()) {
            for(auto i: range(kNumNeighbors)) {
                coordinate_type from = _topo.to_coordinate(i);
                size_t j;
                if(!_topo.try_move(from, i, &j)) {

                }
            }
        }

    }

    void collapse(const wave<T>& w, const std::vector<float>& weights) const {

    }
    
};

template <typename T, size_t N>
struct basic_model {
    typedef typename topology<N>::value_type shape_type;
   


    //template <typename OutputIt>
    void solve(const shape_type& d_shape) {
        wave_propagator<T, N> prop;
        prop.run(this);
    }
};

int main(int argc, char** argv) {
    DEBUG_VALUE_OF(topology<1>::sides);
    DEBUG_VALUE_OF(topology<2>::sides);
    DEBUG_VALUE_OF(topology<3>::sides);
    [[maybe_unused]] int tiles[] = { 101, 102, 203 };
    std::unordered_map<int, char> index = { { 101, 'x' }, { 102, '.' }, {103,'-'} };
    std::vector<int> example = { 101, 101 };
    wave<uint64_t> w;
    adjacency_list<uint64_t, 3> a(3);
    a.add_example(example.begin(), example.end(), topology<3> { { 3,3,3} }); 
    wave_propagator<uint64_t, 3> wp;
    wp.propagate(0, w, a);
    return 0;
}
