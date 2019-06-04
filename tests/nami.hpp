#pragma once

#include <core/random.hpp>
#include <core/ndarray.hpp>
#include <core/range.hpp>
#include <core/bits.hpp>
#include <core/delegate.hpp>

namespace nami {

    template <size_t N>
    struct topology;
    template <typename T>
    struct wave;

    template <typename T, size_t N>
    struct basic_constraint {
        virtual ~basic_constraint() = default;
        virtual void init(wave<T>&, const topology<N>&, std::vector<size_t>*) = 0;
    };

    using wee::range;
    using wee::random;
    using wee::popcount;
    using wee::ctz;
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

template <typename T, size_t N>
struct wave_propagator;

template <typename T, size_t N>
struct constraint {
    virtual ~constraint() = default;
    virtual void init(const wave_propagator<T, N>&) = 0;
    virtual void check(const wave_propagator<T, N>&) = 0;
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
public:

    tileset() {
        push(0, tile_rotation::identity());
    }

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
            DEBUG_VALUE_OF(t);
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
template <size_t N>
struct topology {
    using coordinates   = std::valarray<int>;
    using shape         = std::valarray<int>;
    using directions    = std::valarray<int>;
    shape _shape;
    std::vector<bool> _periodic;
    directions _neighbor;

    topology() {
    }

    topology(const shape& s, bool isPeriodic = false) 
        : _shape(s)
    {
        _neighbor = topology::build_directions(num_dimensions());
    }


    static directions build_directions(size_t kNumDimensions) {
        size_t kNumNeighbors = kNumDimensions << 1;
        size_t cols = kNumDimensions;
        size_t rows = kNumNeighbors;
        auto res = directions(rows * cols);
        size_t x = (rows * cols) >> 1;
        res[std::gslice(0, {rows >> 1, cols}, {cols + 1, 0})] = 1;  // trace of top half
        res[std::gslice(x, {rows >> 1, cols}, {cols + 1, 0})] = -1;  // trace of bottom hals
        return res;

    }

    size_t index_of(const coordinates& c) const { return wee::linearize(c, _shape); }

    coordinates coordinates_of(size_t i) const { return wee::delinearize<int>(i, _shape); }

    static bool is_valid(const coordinates& c, const shape& s) {
        for (auto i : range(s.size())) {
            if (c[i] < 0 || s[i] <= c[i]) {
                return false;
            }
        }
        return true;
    }

    bool is_valid(const coordinates& c) {
        return is_valid(c, _shape);
    }

    bool try_move(size_t i, const directions& d, size_t* i_out) const {
        return try_move(coordinates_of(i), d, i_out);
    }

    bool try_move(const coordinates& from, const directions& d, size_t* i_out) const {
        auto to = from + d;
        if (is_valid(to, _shape)) {
            return *i_out = index_of(to), true;
        }
        return false;
    }

    size_t num_dimensions() const { return _shape.size(); }

    size_t num_neighbors() const { return num_dimensions() << 1; }

    directions neighbor(size_t i) const {
        return _neighbor[std::slice(i * num_dimensions(), num_dimensions(), 1)];
    }
};

template <typename T>
struct wave {
    std::vector<T> _coeff;
    T _blacklist;


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

    std::vector<size_t> avail(const T& t) {
        auto tmp = t;
        std::vector<size_t> opts(popcount(tmp));
        for (auto i : range(opts.size())) {
            opts[i] = ctz(tmp);  // index_of(tmp);
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
template <typename T, size_t N>
struct basic_model {
    typedef std::unordered_map<size_t, uint64_t> adjacency_list_type;
    adjacency_list_type _adjacency;
    tileset _ts;
    std::unordered_map<int, int> _index;
    topology<N> _topo;
    T _banned;

    typedef wee::event_handler<void(const std::vector<int>&)> cb_t;
    cb_t on_done;
    cb_t on_update;

    std::vector<basic_constraint<T, N>*> _constraints;
    
    //std::vector<float> _weights;
    

    basic_model(const tileset& ts, size_t);

    const tileset& tiles() const { return _ts; }

    void add_constraint(basic_constraint<T,N>* c) { _constraints.push_back(c); }

    /**
     * return the effective domain of this model
     */
    T domain() const {
        uint64_t res = 0;
        for(auto i : range(_ts.size())) {
            res |= bitmaskof<uint64_t>(_ts.tile_to_index(_ts.tile(i))); // <???
        }
        return res & ~_banned;
    }

    void ban(T t) { _banned |= bitmaskof<T>(t); }


    template <typename OutputIt>
    void weights(OutputIt it, bool normalize = false) {
        std::vector<float> f;
        for([[maybe_unused]] auto i: range(_ts.size())) {
            //f.push_back(1.0f);
            f.push_back(_ts.frequency_of(_ts.tile(i)));
        }
        DEBUG_VALUE_OF(f);
        if(normalize) {
            float t = std::inner_product(f.begin(), f.end(), f.begin(), 0.0f);
            float reciprocal = 1.0f / t;
            for(auto i : range(f.size())) {
                f[i] = f[i] * reciprocal;
            }
        }
        std::copy(f.begin(), f.end(), it);
    }

    void add_adjacency(const std::vector<int>& from, const std::vector<int>& to, const typename topology<N>::directions& d, const tile_rotation& = tile_rotation::identity());
    void add_adjacency(int a, int b, const typename topology<N>::directions& d, const tile_rotation& = tile_rotation::identity() );
    void add_adjacency(int a, int b, size_t i,const tile_rotation& = tile_rotation::identity() );
    //void weights_from_example(const int* data, size_t len);
    void add_example(const int* data, const typename topology<N>::shape& shape);
    void solve_for(const typename topology<N>::shape&);
};

/**
 * TODO 2019-05-15: create adjacency list struct
 */
#if 0
struct adjacency_list {
    topology _topo;
    std::unordered_map<int, int> _index;
    
    adjacency_list(size_t nd) {
        _topo = topology(topology::shape(3, nd));
        for(auto n: range(nd << 1)) {
            _index.insert({_topo.index_of(_topo.neighbor(n) + 1), n});

        }
    }
};
#endif
template <typename T, size_t N>
struct wave_propagator {
    
    wee::random _randgen = { 600089286 };
    size_t _len;

    typedef wee::event_handler<void(const wave<T>&)> cb_t;
    cb_t on_progress;
    cb_t on_done;

    wave_propagator() { //size_t len) : _len(len) {
        
        //_randgen.reset(wee::randgen((uint32_t){}, (uint32_t)78612512));
        DEBUG_VALUE_OF(_randgen.seed());
    }

    size_t min_entropy(wave<T>* _wave) {
        size_t ret = 0;
        float min_h = std::numeric_limits<float>::infinity();
        for (size_t i : range(_wave->_coeff.size())) {
            if (_wave->collapsed_at(i)) 
                continue;

            float h =
                entropyof(_wave->at(i)) - _randgen.next<float>(0.0f, 1.0f) / 1000.0f;
            if (h < min_h) {
                min_h = h;
                ret = i;
            }
        }
        return ret;
    }

    bool collapse(wave<T>* _wave, size_t i, const std::vector<float>& weights) { 
        //auto i = at.x + at.y * _size.x;
        
        std::unordered_map<int, float> w;
        //std::map<int, float> w; 
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
                _wave->at(i) = bitmaskof<uint64_t>(key);
                return true;
            }
        }
        return false;
    }

    void propagate(wave<T>* _wave, size_t at, const topology<N>& topo, const typename basic_model<T,N>::adjacency_list_type& adjacency) { 
        std::vector<size_t> open = { at };

        while(!open.empty()) {
            size_t cur_i = open.back();
            open.pop_back();
            //auto cur_coords = delinearize<int>(cur_i, _output_shape);
            auto cur_coords = topo.coordinates_of(cur_i);

            //bitmask_t cur_bitmask = _coefficients[cur_i];
            auto cur_bitmask = _wave->at(cur_i);
            auto cur_avail = _wave->avail(cur_bitmask);

            for(size_t i=0; i < topo.num_neighbors(); i++) { 
                auto d = topo.neighbor(i);
                size_t other_i;
                if(!topo.try_move(cur_i, d, &other_i)) {
                    continue;
                }
               
                auto opts = 0;
                for(auto ct: cur_avail) {
                    auto idx = ct * topo.num_neighbors() + i;
                    if(adjacency.count(idx)) {
                        opts |= adjacency.at(idx);//ct * topo.num_neighbors() + i);
                    }
                }
                /**
                 * this next line is important! This prevents the selection of tiles that have 0 neighbors 
                 */
                if(!opts) 
                    continue;
                
                auto any_possible = _wave->at(other_i) & opts;
                if(!any_possible) {
                    //reset(&_initial[0], _initial.size());
                    return;
                }
                //!
                //! TODO: shoudn't this be '&'
                //! as in if((_coefficients[other_i] & any_possible) != 0)
                //!
                if(_wave->at(other_i) != any_possible) {
                    open.push_back(other_i);//coords);
                }
                //_coefficients[other_i] = any_possible;
                _wave->at(other_i) = any_possible;
            }
        }
    }

    void step(wave<T>* _wave, const topology<N>& topo, const auto& weights, const typename basic_model<T, N>::adjacency_list_type& adjacency) {
        size_t i = min_entropy(_wave);
        if(collapse(_wave, i, weights)) {
            propagate(_wave, i, topo, adjacency);
        }
        on_progress(*_wave);
    }

    
    void run(basic_model<T,N>* m, const typename topology<N>::shape& s) {
        topology<N> topo {s};
        std::vector<float> weights;
        m->weights(std::back_inserter(weights));
        wave<T> current(array_product(s), m->domain());

        for(auto* c: m->_constraints) {
            std::vector<size_t> affected;
            c->init(current, s, &affected);
            for(auto i: affected) {
                propagate(&current, i, topo, m->_adjacency);
            }
        }

        while(!current.did_collapse()) {
            step(&current, topo, weights, m->_adjacency);
        }
        on_done(current);
    }
};

template <typename T, size_t N>
basic_model<T,N>::basic_model(const tileset& ts, size_t n) 
    : _ts(ts)
    , _banned(0)
{
    if(_ts.size() == 0) {
        throw std::runtime_error("tileset is empty!");
    }
    _topo = topology<N>(typename topology<N>::shape(3, n)); 
    for(auto n: range(n << 1)) {
        _index.insert({_topo.index_of(_topo.neighbor(n) + 1), n});
    }
}

template <typename T, size_t N>
void basic_model<T,N>::add_adjacency(const std::vector<int>& from, 
        const std::vector<int>& to, 
        const typename topology<N>::directions& d,
        const tile_rotation& r) 
{
    assert(_topo.num_neighbors() == d.size());
    for(auto a : from) { 
        for(auto b : to) {
            add_adjacency(a, b, d, r);
        }
    }
}
template <typename T, size_t N>
void basic_model<T,N>::add_adjacency(int a, int b, size_t i, const tile_rotation& r) {
    //size_t num_neighbors = _index.size();
    size_t idx = i + r.cw;
    size_t y = _ts.tile_to_index(a);

    _adjacency[y *_topo.num_neighbors() + idx] |= bitmaskof<uint64_t>(_ts.tile_to_index(b) + r.cw); 
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
template <typename T, size_t N>
void basic_model<T,N>::add_adjacency(int a, int b, const typename topology<N>::directions& d, const tile_rotation& r) {
    assert(_topo.num_neighbors() == d.size());
    typename topology<N>::shape s(3, d.size());
    auto dt = typename topology<N>::directions(d + 1); // offset towards "center" of matrix
    add_adjacency(a, b, _index[linearize(dt, s)], r);
}

template <typename T, size_t N>
void basic_model<T,N>::add_example(const int* data, const typename topology<N>::shape& shape) {
    assert(_topo.num_dimensions() == shape.size());
    topology<N> local(shape);    
    size_t len = array_product(shape);
    for(auto i: range(len)) {
        auto sample_coord = local.coordinates_of(i);//delinearize<int>(i, shape);
        for(auto n: range(local.num_neighbors())) {
            auto d = local.neighbor(n);
            size_t j;
            if(local.try_move(sample_coord, d, &j)) {
                add_adjacency(data[i], data[j], n);
            }
        }
    }
    DEBUG_VALUE_OF(_adjacency);
}



template <typename T, size_t N>
void basic_model<T,N>::solve_for(const typename topology<N>::shape& s) {
    assert(_topo.num_dimensions() == s.size());
    wave_propagator<T, N> wp;


    /*wp.on_progress += [] (const wave<uint64_t>& w) {
        DEBUG_VALUE_OF(w.progress());
    };*/

    wp.on_done += [this] (const wave<uint64_t>& w) {
        std::vector<int> res;
        //copy_as_tiles(w._coeff.begin(), w._coeff.end(), std::back_inserter(res));
        std::transform(
            w._coeff.begin(), 
            w._coeff.end(), 
            std::back_inserter(res), 
            [this] (uint64_t i) { 
                return _ts.tile(indexof(i)); 
            } 
        );
        this->on_done(res);
    };

    std::vector<int> res;
    wp.run(this, s);//topology {s});
}

}  // namespace nami
