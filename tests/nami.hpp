#pragma once

#include <core/random.hpp>
#include <core/ndarray.hpp>
#include <core/range.hpp>
#include <core/bits.hpp>
#include <core/delegate.hpp>

template <typename T, size_t N, size_t... Is>
size_t linearize_array_impl(const wee::ndindexer<N>& ix, const T& ary, std::index_sequence<Is...>) {
    return ix.linearize((ary[Is])...);
}

template <typename T, size_t N>
size_t linearize_array(const wee::ndindexer<N>& ix, const T& ary) {
    return linearize_array_impl<T, N>(ix, ary, std::make_index_sequence<N>()); 
}

template <typename T>
std::ostream& operator << (std::ostream& os, const std::valarray<T>& va) {
    os << "[";
    for(auto it=std::begin(va); it != std::end(va) - 1; it++) {
        os << *it << ", ";
    }
    return os << *std::end(va) << "]";
}

namespace nami {

    using wee::range;
    using wee::random;

template <typename T>
constexpr static int indexof(const T& t) {
    return ctz(t);
}

template <typename T>
constexpr static T bitmaskof(size_t i) {
    return static_cast<T>(1) << i;
}
template <typename T>
float entropyof(const T& t) {
    return 1.0f - 1.0f / static_cast<float>(popcount(t));
}
template <typename S>
size_t array_product(const S& a) {
    return std::accumulate(std::begin(a), std::end(a), 1,
                           std::multiplies<int>());
}

template <size_t N, typename T>
struct wave_propagator;

template <size_t>
struct topology;

template <size_t N, typename T>
struct basic_constraint {
    virtual ~basic_constraint() = default;
    virtual void init(const wave_propagator<N,T>&, const topology<N>&, std::vector<size_t>*) const = 0;
    virtual void check(const wave_propagator<N,T>&, const topology<N>&, std::vector<size_t>*) const = 0;
};


enum class tile_symmetry : uint8_t {
    /**
     * no symmetry
     */
    F = 0,
    /*
     * fully symmetric
     */
    X, 
    /**
     * reflectable on y-axis
     */
    T, 
    /**
     * reflectable on x-axis and y-axis
     */
    I, 
    /**
     * reflectable on one diagonal
     */
    L, 
    /**
     * reflectable on two diagonals
     */
    slash, 
    /**
     * reflectable on other diagonal
     */
    Q, 
    /**
     * rotatable by 180 degrees
     */
    N, 
    /**
     * reflectable on x-axis
     */
    E
};

struct tile_rotation {
    int     cw;
    bool    reflect_x;

    constexpr static const tile_rotation identity() { return tile_rotation { 0, false }; }
};

struct tileset {
    typedef int tile_type;
    std::vector<tile_type> _data;
    std::unordered_map<tile_type, size_t> _index;
    std::vector<size_t> _frequency;
    //std::vector<tile_rotation> _rotations;

    tileset() {
        push(0, tile_rotation::identity());
    }

    const auto& tiles() { return _data; }

    void set_frequency(tile_type t, size_t f) {
        _frequency[tile_to_index(t)] = f;
    }

    size_t size() const  {
        return _data.size();
    }

    size_t push(const tile_type& t, const tile_rotation&) { 
        
        return push_impl(t);

        /*std::function<int(int)> a, b;
        int cardinality;

        if(sym == 'L') { 
            cardinality = 4; 
            a = [] (int i) { return (i + 1) % 4; };
            b = [] (int i) { return ((i % 2) == 0) ? i + 1 : i - 1; };
        } else if (sym == 'T') {
            cardinality = 4;
            a = [] (int i) { return (i + 1) % 4; };
            b = [] (int i) { return ((i % 2) == 0) ? i : 4 - 1; };
        } else if(sym == 'I') {
            cardinality = 2;
            a = [] (int i) { return 1 - i; };
            b = [] (int i) { return i; };
        } else if(sym == '/') {
            cardinality = 2;
            a = [] (int i) { return 1 - i; };
            b = [] (int i) { return 1 - i; };
        } else {
            cardinality = 1;
            a = [] (int i) { return i; };
            b = [] (int i) { return i; };
        }

        for([[maybe_unused]] auto i: range(cardinality)) {
            DEBUG_VALUE_OF(t);
            DEBUG_VALUE_OF(a(t));
            DEBUG_VALUE_OF(a(a(t)));
            DEBUG_VALUE_OF(a(a(a(t))));
            DEBUG_VALUE_OF(b(t));
            DEBUG_VALUE_OF(b(a(t)));
            DEBUG_VALUE_OF(b(a(a(t))));
            DEBUG_VALUE_OF(b(a(a(a(t)))));
        }*/
    }

    size_t push_impl(const tile_type& t) {
        if(_index.count(t) == 0) {
            _index[t] = _data.size();
            _data.push_back(t);
            _frequency.push_back(0);
        }
        return _frequency[tile_to_index(t)]++;
    }

    const size_t frequency_of(const tile_type& t) const {
        return frequency_at(tile_to_index(t));
    }

    const size_t frequency_at(size_t i) const {
        return _frequency[i];
    }
    

    size_t tile_to_index(const tile_type& t) const { 
        if(_index.count(t) == 0) {
            throw std::out_of_range("tile is not in set");
        }
        return _index.at(t); 
    }

    const tile_type& tile(const size_t i) const { return _data[i]; }

    static tileset from_example(const tile_type* data, size_t len) {
        std::vector<tile_type> t(data, data + len);
        tileset res;
        for(const auto& it: t) {
            res.push(it, tile_rotation::identity());
        }
        return res;
    }
};

template <typename T, size_t N>
std::array<T, N> operator + (const std::array<T, N>& a, const std::array<T, N>& b) {
    std::array<T, N> res;
    std::transform(a.begin(), a.end(), b.begin(), res.begin(), std::plus<T>());
    return res;
}
template <typename T, size_t N>
std::array<T, N> operator - (const std::array<T, N>& a, const std::array<T, N>& b) {
    std::array<T, N> res;
    std::transform(a.begin(), a.end(), b.begin(), res.begin(), std::minus<T>());
    return res;
}

template <size_t N>
struct topology {
    using coordinates   = std::array<ptrdiff_t, N>;//std::valarray<int>;
    using shape_t       = std::array<ptrdiff_t, N>;//std::valarray<int>;
    using directions    = std::array<ptrdiff_t, N>;//std::valarray<int>;
        constexpr static const size_t kNumDimensions = N;
        constexpr static const size_t kNumNeighbors = kNumDimensions << 1;
        constexpr static const size_t M = kNumDimensions * kNumNeighbors;

    wee::ndindexer<N> _ix;

    std::vector<bool> _periodic;
    std::array<ptrdiff_t, M> _neighbors;

    topology() {
    }

    topology(const shape_t& s, bool isPeriodic = false) 
        : _ix({s})
    {
        _neighbors = topology::build_directions();
    }

    template <typename T>
    static void trace(T* ptr, size_t start, size_t n, size_t stride, T val) {
        for(size_t i=0; i < n; i+=stride) {
            ptr[start + i] = val;
        }
    }

    static std::array<ptrdiff_t, M> build_directions() {
        std::array<ptrdiff_t, M> res = { 0 };
        trace<ptrdiff_t>(&res[0], 0,     M >> 1, N + 1,  1);
        trace<ptrdiff_t>(&res[0], N * N, M >> 1, N + 1, -1);
        return res;

    }

    const wee::ndindexer<N>& indexer() const { return _ix; }

    size_t index_of(const coordinates& c) const { 
        return linearize_array(indexer(), c);
//        return indexer().linearize(c); 
    }

    coordinates coordinates_of(size_t i) const { return indexer().delinearize(i); }

    static bool is_valid(const topology& topo, const coordinates& c) {
        for (auto i : range(N)) {
            if (c[i] < 0 || topo.indexer().shape()[i] <= c[i]) {
                return false;
            }
        }
        return true;
    }


    bool try_move(size_t i, const directions& d, size_t* i_out) const {
        return try_move(coordinates_of(i), d, i_out);
    }

    bool try_move(const coordinates& from, const directions& d, size_t* i_out) const {
        auto to = from + d;
        if (is_valid(*this, to)) {
            return *i_out = index_of(to), true;
        }
        return false;
    }

    constexpr size_t num_dimensions() const { return N; }

    constexpr size_t num_neighbors() const { return num_dimensions() << 1; }

    directions neighbor(size_t i) const {
        //return _neighbor[std::slice(i * num_dimensions(), num_dimensions(), 1)];
        directions res;
        auto first = _neighbors.begin() + i * N;
        auto last = first + N;
        std::copy(first, last, res.begin());
        return res;
    }
};

template <typename T>
struct wave {
    std::vector<T> _coeff;
    T _blacklist;

    std::vector<T>& coefficients() const { return _coeff; }

    float progress() const {
        int num_collapsed =
            std::count_if(_coeff.begin(), _coeff.end(), [](const T& i) { return popcount(i) == 1; });
        return static_cast<float>(num_collapsed) / _coeff.size();
    }

    wave(size_t len, const T& domain) {
        _coeff.resize(len, domain); 
        _blacklist = 0;
    }

    inline bool collapsed_at(size_t i) const {
        return popcount(_coeff[i]) == 1;
    }

    bool did_collapse() const {
        for(auto i : range(_coeff.size())) {
            if(!collapsed_at(i)) {
                return false;
            }
        }
        return true;
    }

    auto avail_at(size_t i) { return avail(at(i)); } 

    /**
     * returns an std::vector containing the tile indices encoded in bitmask `t` of type `T`
     */
    std::vector<size_t> avail(const T& t) {
        auto tmp = t;
        std::vector<size_t> opts(popcount(tmp));
        for (auto i : range(opts.size())) {
            opts[i] = indexof(tmp);  // index_of(tmp);
            auto lb = tmp & -tmp;
            tmp ^= lb;
        }
        return opts;
    }

    const T& at(size_t i) const { return _coeff[i]; }
    T& at(size_t i) { return const_cast<T&>(static_cast<const wave&>(*this).at(i)); }

};

/**
 * TODO 2019-05-15:
 * basic model should have *some* notion of dimensionality, right? I mean.. it seems 
 * weird for the propagator to have ownership of this parameter entirely.
 *
 * Propbably  a template <size_t n> should suffice. Then just assert n == shape.size() or 
 * whatever. The risk to be mitigated is that 2D and 3D examples (adjacencies) get mixed.
 * Also, templates open the option to statically initialize the adjacency list.
 */

template <size_t N, typename T>
struct basic_model {
    typedef std::map<size_t, T> adjacency_list_type; // keep ordered for now... makes life easier to debug
    adjacency_list_type _adjacency;
    tileset _ts;
    std::unordered_map<int, int> _index;
    topology<N> _topo;
    T _banned;
    std::vector<basic_constraint<N, T>* > _constraints;
    typedef wee::event_handler<void(const std::vector<int>&)> cb_t;
    cb_t on_done;
    cb_t on_update;
    
    //std::vector<float> _weights;
    

    basic_model(const tileset& ts);

    const tileset& tiles() const { return _ts; }

    void add_constraint(basic_constraint<N, T>* c) {
        _constraints.push_back(c);
    }

    const decltype(_constraints)& constraints() const {
        return _constraints;
    }

    /**
     * return the effective domain of this model
     */
    T domain() const {
        T res = bitmaskof<T>(_ts.tile_to_index(0));;
        for(auto i : range(_ts.size())) {

#if 1
            if(!_adjacency.count(_ts.tile_to_index(i)))
                continue;
#endif
            res |= bitmaskof<T>(_ts.tile_to_index(i));
        }
        return res & ~_banned;
    }

    void ban(const int id) {
        _banned |= bitmaskof<T>(_ts.tile_to_index(id));

    }

    template <typename OutputIt>
    void weights(OutputIt it, bool normalize = false) {
        std::vector<float> f;
        for([[maybe_unused]] auto i: range(_ts.size())) {
            //f.push_back(1.0f);
            f.push_back(_ts.frequency_of(_ts.tile(i)));
        }
        if(normalize) {
            float t = std::inner_product(f.begin(), f.end(), f.begin(), 0.0f);
            float reciprocal = 1.0f / t;
            for(auto i : range(f.size())) {
                f[i] = f[i] * reciprocal;
            }
        }
        std::copy(f.begin(), f.end(), it);
    }

    void add_adjacency(const std::vector<int>& from, const std::vector<int>& to, size_t d, const tile_rotation& = tile_rotation::identity());
    void add_adjacency(int a, int b, size_t i,bool is_bidirectional = false, const tile_rotation& = tile_rotation::identity());
    //void weights_from_example(const int* data, size_t len);
    void add_example(const int* data, const typename topology<N>::shape_t& shape);
    void solve_for(const typename topology<N>::shape_t&);

    bool has_adjacency(int a, int b, int neighbor_index) {
        constexpr const size_t kNumNeighbors = N << 1;
        int i_a = _ts.tile_to_index(a);
        int i_b = _ts.tile_to_index(b);
        int j   = i_a * kNumNeighbors + neighbor_index;

        if(_adjacency.count(j)) {
            return (_adjacency[j] & bitmaskof<T>(i_b)) != 0;
        }
        return false;
    }
};

/**
 * TODO 2019-05-15: create adjacency list struct
 */
template <size_t N>
struct adjacency_list {
    topology<N> _topo;
    std::unordered_map<int, int> _index;
    
    adjacency_list(size_t nd) {
        _topo = topology(typename topology<N>::shape(3, nd));
        for(auto n: range(nd << 1)) {
            _index.insert({_topo.index_of(_topo.neighbor(n) + 1), n});

        }
    }
};

template <size_t N, typename T>
struct wave_propagator {
    constexpr static const size_t kNumNeighbors = N << 1;
    
    wee::random _randgen;// { 4264159999 };
    size_t _len;


    typedef wee::event_handler<void(const wave<T>&)> cb_t;
    //cb_t on_init;
    cb_t on_update;
    cb_t on_done;

    wave<T>* _current = nullptr;

    wave_propagator() { 
        DEBUG_VALUE_OF(_randgen.seed());
    }
    size_t min_entropy(wave<T>* _wave) {
        size_t ret = 0;
        float min_h = std::numeric_limits<float>::infinity();
        for (size_t i : range(_wave->_coeff.size())) {
            if (_wave->collapsed_at(i)) 
                continue;

            float h =
                entropyof(_wave->at(i)) - _randgen.next<float>(0.0f, 1.0f) / 100.0f;
            if (h < min_h) {
                min_h = h;
                ret = i;
            }
        }
        return ret;
    }
    int collapse(wave<T>* _wave, size_t i, const std::vector<float>& weights) { 
        //auto i = at.x + at.y * _size.x;
        
        //std::map<int, float> w;
        std::unordered_map<int, float> w; 
        float total_weight = 0.0f;
        auto avail = _wave->avail_at(i);//(_coefficients[i]);
        for(auto t: avail) {
            w.insert(std::pair(t, weights[t]));
            total_weight += weights[t];
        }

        float random = _randgen.next<float>(0.f, 1.0f) * total_weight;

        for(const auto& [key, val]: w) {
            random -= val;
            if(random < 0) {
                //_coefficients[i] = _bitmask_of(key);
                _wave->at(i) = bitmaskof<T>(key);
                return 0;
            }
        }
        return -1;
    }
    /**
     * propagate removes all incompatible options from the neighboring cells, it functions as a flood-fill algorithm; which means it starts at
     * one point and recursively iterates over all neighboring cells.
     *
     * for all neighbors of current cell:
     *      for all options in current cell:
     *          remove options from neighbor where current options aren't compatible with the neighbors remaining options
     *       
     */

    T avail_for(const std::vector<size_t>& in_t, size_t at, const typename basic_model<N, T>::adjacency_list_type& adjacency) {
        T res {};
        for (auto i : in_t) {
            size_t offset = i * kNumNeighbors + at;
            if (adjacency.count(offset)) {
                res |= adjacency.at(offset);  // note that the values in `adjacency` are already masks encoding the indices.
            } 
            //else {
            //    res |= 1 << 0;
            //}
        }
        return res;
    }

    int propagate(wave<T>* _wave, size_t at, const topology<N>& topo, const typename basic_model<N, T>::adjacency_list_type& adjacency) { 
        constexpr const size_t kNumNeighbors = N << 1;
        std::vector<size_t> open = { at };

        while(!open.empty()) {
            size_t cur_i = open.back();
            open.pop_back();
            auto cur_coords = topo.coordinates_of(cur_i);
            auto cur_bitmask = _wave->at(cur_i);
            auto cur_avail = _wave->avail(cur_bitmask);

            for(auto neighbor_index : wee::range(kNumNeighbors)) {
                auto d = topo.neighbor(neighbor_index);
                size_t i_neighbor;
                if(!topo.try_move(cur_coords, d, &i_neighbor)) {
                    continue;
                }
                if(_wave->collapsed_at(i_neighbor)) 
                    continue;

                T opts_for_neighbor = avail_for(cur_avail, neighbor_index, adjacency);
                T neighbor_bitmask = _wave->at(i_neighbor);
                /**
                 * if the neighbor doesn't have any options left, we simply move over to the next neighbor.
                 */
                if(opts_for_neighbor == 0) {
                    continue;
                }
                T any_possible = neighbor_bitmask & opts_for_neighbor; 
                if(!any_possible) {
                    //DEBUG_LOG("!!! NOPE !!!");
                    //return -1; // nope!
                    _wave->at(i_neighbor) = 1;
                    continue;

                }
                if(neighbor_bitmask != any_possible) { // if the neighbor didn't change
                    open.push_back(i_neighbor);//coords);
                    _wave->at(i_neighbor) = any_possible;
                }
            }
        }
        return 0;
    }
    int step(wave<T>* _wave, const topology<N>& topo, const auto& weights, const typename basic_model<N, T>::adjacency_list_type& adjacency) {
        size_t i = min_entropy(_wave);
        if(collapse(_wave, i, weights)) {
            throw std::runtime_error("collapse");
        }
        if(propagate(_wave, i, topo, adjacency)) {
            //step(_wave, topo, weights, adjacency);
            throw std::runtime_error("propagate");
        }
        return 1;
    }


    void run(basic_model<N, T>* m, const typename topology<N>::shape_t& s) {
        static int runcount = 10;

        auto topo = topology<N> {s};
        std::vector<float> weights;
        m->weights(std::back_inserter(weights));
        wave<T> wv(array_product(s), m->domain());
        _current = &wv;

        try {

            for(auto* constraint : m->constraints()) {
                std::vector<size_t> affected;

                constraint->init(*this, s, &affected);

                for(auto i: affected) {
                    propagate(_current, i, topo,  m->_adjacency);
                }
            }

            while(!_current->did_collapse()) {
                step(_current, topo, weights, m->_adjacency);

                std::vector<size_t> affected;

                for(auto* constraint : m->constraints()) {
                    constraint->check(*this, s, &affected);
                }

            }
        } catch(...) {
            _randgen.reset(wee::randgen((uint32_t){}, (uint32_t)78612512));
            DEBUG_VALUE_OF(_randgen.seed());
            if(0 < runcount--)
                run(m, s);
            exit(0);
        }
        on_done(wv);
        _current = nullptr;
    }
   
    void collapse_to(size_t i, T pattern) const {
        if(_current != nullptr) {
            _current->_coeff[i] = pattern;
        }
    }
};

template <size_t N, typename T>
basic_model<N,T>::basic_model(const tileset& ts)
    : _ts(ts)
    , _banned(0)
{
    if(_ts.size() == 0) {
        throw std::runtime_error("tileset is empty!");
    }

    
    _topo = topology<N>(typename topology<N>::shape_t { 3 });//typename topology<N>::shape(3, n)); 
    typename topology<N>::directions one = { 1 };
    for(auto i: range(N << 1)) {
        _index.insert({_topo.index_of(_topo.neighbor(i) + one), i});
        //add_adjacency(0, 0, i, tile_rotation::identity());
    }

}

template <size_t N, typename T>
void basic_model<N,T>::add_adjacency(const std::vector<int>& from, 
        const std::vector<int>& to, 
        size_t d, 
        const tile_rotation& r) 
{
    for(auto a : from) { 
        for(auto b : to) {
            add_adjacency(a, b, d, r);
        }
    }
}
/**
 * i is the neighbor index
 */
template <size_t N, typename T>
void basic_model<N,T>::add_adjacency(int a, int b, size_t i, bool is_bidirectional, const tile_rotation& r) {
    constexpr const size_t kNumNeighbors = N << 1;
    //assert(i < kNumNeighbors);
    int i_a = _ts.tile_to_index(a);
    int i_b = _ts.tile_to_index(b);

    _adjacency[i_a * kNumNeighbors + i] |= bitmaskof<T>(i_b);
    if(is_bidirectional != false) {
        size_t j = (i + N) % kNumNeighbors;
        _adjacency[i_b * kNumNeighbors + j] |= bitmaskof<T>(i_a);
    }
}
/**
 * goal: generalized lookup of cartesian coordinates to array subscript for direction only
 *
 * bit of a reach here, but: we know the topology information that we require here
 * based on the size of the direction. The size() of the direction
 * will be the number of dimensions in the target topology.
 *
 * challenge: find the index into the adjacency table based on the cardinal direction
 * provided by @param d.
 * 
 * 2D:
 * DEBUG_VALUE_OF((int)linearize({0, 1}, s) ); // -1,  0 --> +1 = 0, 1
 * DEBUG_VALUE_OF((int)linearize({1, 0}, s) ); //  0, -1 --> +1 = 1, 0
 * DEBUG_VALUE_OF((int)linearize({1, 2}, s) ); //  0,  1 --> +1 = 1, 2
 * DEBUG_VALUE_OF((int)linearize({2, 1}, s) ); //  1,  0 --> +1 = 2, 1
 * 
 * we attempt to map the input quadrant 
 *
 * +-----------+
 * | x | a | x |
 * +---+---+---+
 * | b | x | c |
 * +---+---+---+
 * | x | d | x |
 * +---+---+---+
 *
 * to a n+1 topology.
 *
 * +-----------+
 * | 0 | 1 | 2 |
 * +---+---+---+
 * | 3 | 4 | 5 |
 * +---+---+---+
 * | 6 | 7 | 8 |
 * +---+---+---+
 *
 * check if this is generalizeable to 3D...
 * 
 * turns out that it isn't. 2D maps linearly, i.e.: with increments of 2, 
 * 3D has increments of 4, 6, 2, 1, 1, 2 and 6.
 *
 * In the end, the option was chosen to create a mapping from the tilespace 
 * indices to linear-sequential indices. (_index)
 *
 */
/*
template <size_t N, typename T>
void basic_model<N, T>::add_adjacency(int a, int b, const typename topology<N>::directions& d, const tile_rotation& r) {
    wee::ndindexer<2> ix({ N * 2, N });
    
    auto quad = d;
    float (*fabs)(float) = &std::abs;
    std::transform(quad.begin(), quad.end(), quad.begin(), fabs);

    DEBUG_VALUE_OF(linearize_array(ix, quad));


    //DEBUG_VALUE_OF(linearize_array(ix, d + one));//std::array<ptrdiff_t, 2> { 2,3 } ));

    //assert(_topo.num_dimensions() == d.size());
    //typename topology<N>::shape s(3, d.size());
    //auto dt = d + typename topology<N>::directions {1};//typename topology<N>::directions(d + 1); // offset towards "center" of matrix
    //DEBUG_VALUE_OF(_topo.indexer().linearize(d));
    //add_adjacency(a, b, _index[_topo.indexer().linearize(d)], r);
    //add_adjacency(a, b, _index[_topo.indexer().linearize(d)], r);
}

*/
template <size_t N, typename T>
void basic_model<N, T>::add_example(const int* data, const typename topology<N>::shape_t& shape) {
    assert(_topo.num_dimensions() == shape.size()); // redundant? yes!
    wee::ndindexer<N> ix(shape);
    DEBUG_VALUE_OF(_adjacency);

    topology<N> local(shape);    
    size_t len = array_product(shape);
    for(auto i: range(len)) {
        auto sample_coord = local.coordinates_of(i);
        DEBUG_LOG("******");

        for(auto n: range(local.num_neighbors())) {
            auto d = local.neighbor(n);
            size_t j;
            if(local.try_move(sample_coord, d, &j)) {
                //if(data[i] == 0) continue;
                if(!has_adjacency(data[i], data[j], n)) {
                    DEBUG_VALUE_OF(sample_coord);
                    DEBUG_VALUE_OF(d);
                    DEBUG_VALUE_OF(std::make_tuple(data[i], data[j]));
                    add_adjacency(data[i], data[j], n, false);
                    DEBUG_VALUE_OF(_adjacency);
                } else {
                    //DEBUG_LOG("adjacency exists, skipping...");
                }
            } else {
                //DEBUG_LOG("out of bounds");
            }
        }
    }
}


template <size_t N, typename T>
void basic_model<N, T>::solve_for(const typename topology<N>::shape_t& s) {
    assert(_topo.num_dimensions() == s.size());

    wave_propagator<N, T> _wp;
    _wp.on_done += [this] (const wave<T>& w) {
        std::vector<int> res;
        //copy_as_tiles(w._coeff.begin(), w._coeff.end(), std::back_inserter(res));
        std::transform(
            w._coeff.begin(), 
            w._coeff.end(), 
            std::back_inserter(res), 
            [this] (T i) { 
                return _ts.tile(indexof(i)); 
            } 
        );
        this->on_done(res);
    };


    std::vector<int> res;
    _wp.run(this, s);//topology {s});
}

}  // namespace nami
