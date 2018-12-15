#include <stack>
#include <tuple>
#include <cmath>
#include <map>
#include <unordered_map>
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
    : _shape(shape)
    , _data(data) 
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
/*
struct WaveFunctionCollapse {
    void run() {
        patterns_from_sample();
        build_propagator();
        while(bool done = false; !done) {
            observe();
            propagate();
        }
        output_observations();
    }

    void patterns_from_sample() {}
    void build_propagator() {}
    void observe() {
        find_lowest_entropy();

    }
};
*/

template <typename T>
auto to_bitmask = [] (size_t index) {
    return static_cast<T>(1 << index);
};

template <typename bitmask_t>
[[maybe_unused]] auto to_index = [] (const bitmask_t& b) {
    //DEBUG_VALUE_OF(__builtin_ctzl(1UL << 33));
    return __builtin_ctzl(b);
    //bitmask_t bm = b;
    //unsigned r = 0;
    //while(bm >>= 1) r++;
    //return r;
};

template <typename It>
auto random_from(It begin, It end) {
    auto len = std::distance(begin, end);
    auto rndi = static_cast<decltype(len)>(wee::randf(0.f, static_cast<float>(len)));
    auto it = begin;
    std::advance(it, rndi);
    return it; 
};

void wfc(const int* in_map, const int2& in_size, int* , const int2& out_size) {
    /**
     * get all unique tiles + their frequencies
     */
    typedef uint64_t bitmask_t;
    constexpr int kNumDimensions = 2;
    constexpr int kNumEdges = kNumDimensions << 1; 
    struct tile { 
        bitmask_t sides[kNumEdges]; 
        int freq; 
        size_t tidx;
    };
    auto print = [] (bitmask_t* a, int w, int h) {
        for(auto y: range(h)) {
            for(auto x: range(w)) {
                std::cout << a[x + y * w] << ", ";

            }
            std::cout << std::endl;
        }
    };
    
    /**
     * Step 1: read the map, and for each input tile; register it and count it's frequency.
     */
    int n = in_size.x * in_size.y;
    std::vector<int> tileset(in_map, in_map + n);
    std::sort(tileset.begin(), tileset.end());
    tileset.erase(std::unique(tileset.begin(), tileset.end()), tileset.end());

    DEBUG_VALUE_OF(tileset);
    /**
     * Step 2: Construct the valid neigbor bitmasks
     */
    [[maybe_unused]] constexpr int2 neighbors[kNumEdges] = { 
        { 0,  1}, // top
        { 1,  0}, // right
        { 0, -1}, // bottom
        {-1,  0}, // left
    };


    std::unordered_map<int, int> lookup;
    for(size_t i=0; i < tileset.size(); i++) {
        lookup[tileset[i]] = i;
    }

    DEBUG_VALUE_OF(lookup);

    /**
     * adjacency stores bitmask of possible combinations for each edge / hyperplane
     */
    std::vector<bitmask_t> adjacency(tileset.size() * 4); 
    for(int y=0; y < in_size.y; y++) {
        for(int x=0; x < in_size.x; x++) {
            int ix0 = x + y * in_size.x;
            int self = in_map[ix0];

            for(int z=0; z < kNumEdges; z++) {
                const int2& n = neighbors[z];
                int2 p = {
                    (x + n.x + in_size.x) % in_size.x,
                    (y + n.y + in_size.y) % in_size.y
                };
                int ix1 = p.x + p.y * in_size.y;
                if(ix1 != ix0) { // happens for ND-1 cases.
                    int nt = in_map[p.x + p.y * in_size.x];
                    adjacency[lookup[self] * 4 + z] |= to_bitmask<bitmask_t>(lookup[nt]);
                } else {
                    adjacency[lookup[self] * 4 + z] = -1;//to_bitmask<bitmask_t>(-1);
                }
            }
        }
    }

    DEBUG_VALUE_OF(adjacency);

    /**
     * construct an initial mask of possibilities by iterating over the tileset.
     */
    bitmask_t initial_mask = 0;
    for(auto it : lookup) {
        DEBUG_VALUE_OF(it.second);
        initial_mask |= to_bitmask<bitmask_t>(it.second);
    }

    DEBUG_VALUE_OF(initial_mask);

    size_t out_n = out_size.x * out_size.y;
    //
    // cells store the bitmask of the possible indices into the tileset.
    //
    std::vector<bitmask_t> cells(out_n);
    std::generate(std::begin(cells), std::end(cells), [&] () { return initial_mask; }); // everything is possible...
    
    int cix = 1;//static_cast<int>(wee::randf(0, out_size.x - 1) + 0.5f); // index of random cell on the N+1D grid.
    int ciy = 0;//static_cast<int>(wee::randf(0, out_size.y - 1) + 0.5f); // index of random cell on the N+1D grid.
    
    DEBUG_VALUE_OF(cix);
    DEBUG_VALUE_OF(ciy);

    int ci = cix + ciy * out_size.x;

    std::vector<int> open;

    [[maybe_unused]] bitmask_t& cell = cells[ci];
    [[maybe_unused]] int rn = 0;// lookup[*random_from(tileset.begin(), tileset.end())];
    cell = to_bitmask<bitmask_t>(rn); //pick a random value from the set of unique tiles.
    //print(&cells[0], 4, 4);

    /**
     * reduce the possibilities of the neightbors, based on the compatibility with the newly set
     * cell.
     */
    for(int i=0; i < kNumEdges; i++) {
        const int2& n = neighbors[i];
        int2 p = {
            (cix + n.x + out_size.x) % out_size.x,
            (ciy + n.y + out_size.y) % out_size.y
        };
        
        bitmask_t& neighbor = cells[p.x + p.y * out_size.x];
        int cell_to_index = to_index<bitmask_t>(cell);

        neighbor &= adjacency[cell_to_index * 4 + i]; // 111 & 010 = 010 vs. 111 & ~010(101) = 101
        if(popcount(neighbor) > 1) {
            open.push_back(p.x + p.y * out_size.x);
        }
    }
    DEBUG_VALUE_OF(open);
    std::vector<bitmask_t> entropies(out_n);
    for(size_t i=0; i < out_n; i++) {
        entropies[i] = popcount(cells[i]);
    }


    print(&entropies[0], out_size.x, out_size.y); 
    

    //te//insor<int, 3> board(
}


template <typename T>
tensor<T, 1> arange(T t) {
    tensor<T, 1> res(t);
    std::iota(std::begin(res), std::end(res), 0);
    return res;
}
int main(int, char**) {

    int in_map[] = { 
        7, 8, 9
    };

    DEBUG_VALUE_OF(__builtin_ctzl(1UL << 33));

    constexpr size_t kOutputDimension = 4;
    int* out_map = new int[kOutputDimension * kOutputDimension];

    wfc(in_map, { 3, 1 }, out_map, { kOutputDimension, kOutputDimension });

    /*auto t = arange(3 * 3 * 3).reshape(3, 3, 3);

    auto va = t.values();
    DEBUG_VALUE_OF(va);
    auto ar = t.slice(1, { 3 }, {3 * 3});

    // iterate through z-dimension at [1, 0]
    for(auto it = std::begin(ar); it != std::end(ar); it++) {
        DEBUG_VALUE_OF(*it);
    }*/

    return 0;
}
