#pragma once

#include <core/random.hpp>
#include <core/ndarray.hpp>
#include <core/range.hpp>
#include <core/bits.hpp>

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

struct tileset {

    struct tile_info {
        int id;
        // rotation
        // reflection
        float frequency;
    };

    typedef int tile_type;
    std::vector<tile_type> _data;
    std::unordered_map<tile_type, size_t> _index;
    std::vector<size_t> _frequency;

    size_t size() const  {
        return _data.size();
    }

    size_t push(const tile_type& t) {
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
        std::sort(t.begin(), t.end());
        t.erase(std::unique(t.begin(), t.end()), t.end());
        tileset res;
        for(const auto& it: t) {
            res.push(it);
        }
        return res;
    }
};

struct topology {
    using coordinates = std::valarray<int>;
    using shape= std::valarray<int>;
    using directions = std::valarray<int>;
    shape _shape;
    std::vector<bool> _periodic;
    directions _axis;

    topology(const shape& s, bool isPeriodic = false) 
        : _shape(s)
    {
        _axis = topology::build_directions(num_dimensions());
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

    size_t index_of(const coordinates& c) const { return linearize(c, _shape); }

    coordinates coordinates_of(size_t i) const { return delinearize<int>(i, _shape); }

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
        return _axis[std::slice(i * num_dimensions(), num_dimensions(), 1)];
    }
};

template <typename T>
struct wave {
    std::vector<T> _coeff;
    T _blacklist;

    wave(size_t len) {
        _coeff.resize(len);
        _blacklist = 0;
    }

    bool collapsed_at(size_t i) const {
        return popcount(_coeff[i]) == 1;
    }

    bool did_collapse() const {
        for(auto i : range(_coeff.size())) {
        //for(size_t i = 0; i < _coeff.size(); i++) {
            if(collapsed_at(i)) {
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

struct basic_model {
    typedef std::unordered_map<size_t, uint64_t> adjacency_list_type;
    adjacency_list_type _adjacency;
    tileset _ts;
    //std::vector<float> _weights;
    
    template <typename S>
    size_t array_product(const std::valarray<S>& a) {
        return std::accumulate(std::begin(a), std::end(a), 1,
                               std::multiplies<int>());
    }

    basic_model(const tileset& ts);

    template <typename OutputIt>
    void weights(OutputIt it, bool normalize = false) {
        std::vector<float> f;
        for(auto i: range(_ts.size())) {
            f.push_back(_ts.frequency_at(i));
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

    void add_adjacency(const std::vector<int>& from, 
            const std::vector<int>& to, 
            const topology::directions& d);
    void add_adjacency(int a, int b, const topology::directions& d);
    //void weights_from_example(const int* data, size_t len);
    void add_example(const int* data, const topology::shape& shape);
    void solve_for(const topology::shape&);
};

template <typename T>
struct wave_propagator {

    wee::random _randgen;
    wave<T>* _wave;

    wave_propagator(size_t len) {
        _wave = new wave<T>(len);
    }

    size_t min_entropy() {
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

    bool collapse(size_t i, const std::vector<float>& weights) { 
        //auto i = at.x + at.y * _size.x;
        
        std::map<int, float> w; 
        float total_weight = 0.0f;
        //auto avail_at = avail(_coefficients[i]);
        for(auto t: _wave->avail_at(i)) {
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

    void propagate(size_t at, const topology& topo, const basic_model::adjacency_list_type& adjacency) { 
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
                //size_t start = i * kNumDimensions;
                //std::valarray<int> d = _neighbors[std::slice(start, kNumDimensions, 1)];
                auto d = topo.neighbor(i);
                size_t other_i;
                if(!topo.try_move(cur_coords, d, &other_i)) {
                    continue;
                }
                //auto other_coords = cur_coords + d;
                //if(!is_valid(other_coords, _output_shape)) 
                //    continue;
                
                //size_t other_i = linearize(other_coords, _output_shape);
                
                auto opts = 0;
                for(auto ct: cur_avail) {
                    //opts |= _adjacency[ct * kNumNeighbors + i];
                    opts |= adjacency.at(ct * topo.num_neighbors() + i);
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

    void step(const topology& topo, const auto& weights, const basic_model::adjacency_list_type& adjacency) {
        size_t i = min_entropy();
        if(collapse(i, weights)) {
            propagate(i, topo, adjacency);
        }
    }

    void run(basic_model* m, const topology::shape& s) {
        topology topo {s};
        std::vector<float> weights;
        m->weights(std::back_inserter(weights));
        //basic_model::adjacency_list_type& adjacency = m->_adjacency;
        while(!_wave->did_collapse()) {
            step(topo, weights, m->_adjacency);
        }
    }
};

basic_model::basic_model(const tileset& ts) 
    : _ts(ts)
{
    if(_ts.size() == 0) {
        throw std::runtime_error("tileset is empty!");
    }
    //_adjacency = decltype(_adjacency)(tileset.size() * kNumNeighbors, 0);
}

void basic_model::add_adjacency(const std::vector<int>& from, 
        const std::vector<int>& to, 
        const topology::directions& d) {
    for(auto a : from) { 
        for(auto b : to) {
            add_adjacency(a, b, d);
        }
    }
}

void basic_model::add_adjacency(int a, int b, const topology::directions& d) {
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
     */
    topology::shape s(3, d.size());
    auto dt = topology::directions(d + 1);
    DEBUG_VALUE_OF((linearize(dt, s)));
    
    _adjacency[_ts.tile_to_index(a)] |= bitmaskof<uint64_t>(_ts.tile_to_index(b)); 
}

/*
void basic_model::weights_from_example(const int* data, size_t len) {
    std::multiset<int> temp(data, data + len);
    for(auto it = temp.begin(); it != temp.end(); it = temp.upper_bound(*it)) {
        size_t tile_index = _ts.tile_to_index(*it);
        _weights[tile_index] = temp.count(*it);
    }
}*/


void basic_model::add_example(const int* data, const topology::shape& shape) {
    topology local(shape);

    topology tilespace(topology::shape(3, shape.size()));
    std::unordered_map<int, int> index;
    for(auto n: range(shape.size() << 1)) {
        index.insert({tilespace.index_of(tilespace.neighbor(n) + 1), n});

    }
    DEBUG_VALUE_OF(index);
    exit(1);
    
    size_t len = array_product(shape);
    for(auto i: range(len)) {
        DEBUG_VALUE_OF(i);
        auto sample_coord = local.coordinates_of(i);//delinearize<int>(i, shape);
        for(auto n: range(local.num_neighbors())) {
            auto d = local.neighbor(n);
            size_t j;
            //DEBUG_VALUE_OF(d);
            if(local.try_move(sample_coord, d, &j)) {
                add_adjacency(data[i], data[j], d);
                 //_adjacency[_ts.tile_to_index(data[i]) * local.num_neighbors() + n] |= 
                 //    bitmaskof<uint64_t>(_ts.tile_to_index(data[j]));
            }
        }
    }
}
void basic_model::solve_for(const topology::shape& s) {
    wave_propagator<uint64_t> wp(array_product(s));
    wp.run(this, s);//topology {s});
}

}  // namespace nami
