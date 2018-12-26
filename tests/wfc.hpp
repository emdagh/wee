#pragma once

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
#include <bitset>
#include <wee/wee.hpp>
#include <wee/core/range.hpp>
#include "attempt3.hpp"
#include "tensor.hpp"

#define RANDOM_SEED     0

using wee::range;

template <typename T, size_t kNumDimensions = 2>
class model {

    size_t _len;
    wee::random _random; // TODO: make the randomness deterministic to facilitate debugging
    // ie.: print the seed on contradictions for example

    typedef tensor<int, kNumDimensions> coord_type;

    //static constexpr size_t kNumDimensions = 2;
    static constexpr size_t kNumNeighbors = kNumDimensions * 2;
    typedef uint64_t bitmask_t;

    std::vector<bitmask_t>     _coefficients;  // bitmask of possible tile values
    std::vector<float>         _weights;       // maps tile index to weight.
    std::vector<bitmask_t>     _adjacency;
    std::unordered_map<T, int> _tile_to_index;
    std::unordered_map<int, T> _index_to_tile;
    int2 _size;
    std::valarray<size_t> _output_shape;
    static constexpr int2 _neighbors[kNumNeighbors] = { 
        { 1,  0}, // right
        { 0,  1}, // bottom
        {-1,  0}, // left
        { 0, -1}, // top    
    };


    std::valarray<int> build_neighbors() {
        constexpr size_t cols = kNumDimensions;
        constexpr size_t rows = kNumNeighbors;
        std::valarray<int> n(rows * cols);
        size_t x = rows * cols / 2;
        n[std::gslice(0, { rows / 2, cols }, {cols + 1, 0})] =  1; // trace of top half
        n[std::gslice(x, { rows / 2, cols }, {cols + 1, 0})] = -1; // trace of bottom hals

        return n;
    }


    size_t _index_of(const bitmask_t& m) {
        return __builtin_ctzll(m); // hard-coded for 64 bit at the moment
    }

    bitmask_t _bitmask_of(size_t i) {
        return 1ULL << i;
    }

    std::vector<size_t> _avail(const bitmask_t& val) { //const int2& at) {
        auto tmp = val;
        
        std::vector<size_t> opts(__popcount(tmp));

        for(auto i: range(opts.size())) {
            opts[i] = _index_of(tmp);
            auto lb = tmp & -tmp;
            tmp ^= lb;
        }
        return opts;
    }

    float _shannon_entropy(size_t at_i) { //const int2& at) {
        //auto at_i = at.x + at.y * _size.x;
#if 1 
        /**
         * is there any good reason why we would use 'real' entropy here?
         * the std::log seems costly for a runtime functionality...
         * 
         * Karth et al. describes: (https://adamsmith.as/papers/wfc_is_constraint_solving_in_the_wild.pdf_
         * 
         * [...]
         * The heuristic of selecting the most constrained
         * variable or equivalently the variable with minimum remaining values
         * (MRV) is well known in constraint solving [23, Chap. 6].
         *[...]
         * [23] Stuart J Russell and Peter Norvig. 2009. Artificial Intelligence: A Modern Approach.
         * Prentice Hall.
         */
        float sum_of_weights = 0.0f;
        float sum_of_weight_log_weights = 0.0f;
        auto opts = _avail(_coefficients[at_i]);
        for(auto i: opts) {
            float w = _weights[i];
            sum_of_weights += w;
            sum_of_weight_log_weights += w * std::log(w);
        }
        return std::log(sum_of_weights) - (sum_of_weight_log_weights / sum_of_weights);
#else
        return 1.0f - 1.0f / static_cast<float>(__popcount(_coefficients[at_i]));
#endif
    }


    void get_min_entropy(size_t* k) { //std::valarray<size_t>* d) {
        float min_H = std::numeric_limits<float>::infinity();

        for(size_t i=0; i < _len; i++) {
            if(__popcount(_coefficients[i]) == 1)
                continue;

            float H = _shannon_entropy(i) - _random.next<float>(0.f, 1.f) / 1000.0f;
            if(H < min_H) {
                min_H = H;
                *k = i;
            }

        }
    }

    void collapse(size_t i) { //const int2& at) {
        //auto i = at.x + at.y * _size.x;
        
        std::map<int, float> w; 
        float total_weight = 0.0f;
        auto avail_at = _avail(_coefficients[i]);
        for(auto t: avail_at) {
            w.insert(std::pair(t, _weights[t]));
            total_weight += _weights[t];
        }

        float random = _random.next<float>(0.f, 1.0f) * total_weight;

        for(const auto& [key, val]: w) {
            random -= val;
            if(random < 0) {
                _coefficients[i] = _bitmask_of(key);
                break;
            }
        }
    }

    bool is_valid(const int2& p, const int2& size) {
        return p.x >= 0 && p.y >= 0 && p.x < size.x && p.y < size.y;
    }
    
    void propagate(size_t at) { //const int2& at) {
        std::vector<size_t> open = { at };

        while(!open.empty()) {
            //int2 cur_coords = open.back();
            size_t cur_i = open.back();
            open.pop_back();
            auto cur_coords = delinearize(cur_i, _output_shape);
            //size_t cur_i = cur_coords.x + cur_coords.y * _size.x;
            bitmask_t cur_bitmask = _coefficients[cur_i];
            auto cur_avail = _avail(cur_bitmask);

            for(size_t i=0; i < kNumNeighbors; i++) { 
                const int2& d = _neighbors[i];
                int2 other_coords = {
                    ((int)cur_coords[1] + d.x),
                    ((int)cur_coords[0] + d.y)
                };

                if(!is_valid(other_coords, _size))
                    continue;

                size_t other_i = other_coords.x + other_coords.y * _size.x;

                bitmask_t opts = 0;
                for(auto ct: cur_avail) {
                    opts |= _adjacency[ct * kNumNeighbors + i];
                }
                
                auto all_options = _coefficients[other_i] & opts;
                if(!all_options)
                    return;
                
                if(_coefficients[other_i] != all_options) {
                    open.push_back(other_i);//coords);
                }
                _coefficients[other_i] = all_options;
            }
        }
    }
   
    bool is_fully_collapsed() {
        for(auto i: _coefficients) {
            if(__popcount(i) > 1) return false;
        }
        return true;
    }

    void build_adjacency(const T* in_map, const int2& in_size, decltype(_adjacency)& res) {
        std::fill(res.begin(), res.end(), 0);
        std::valarray<size_t> in_shape = {
            (size_t)in_size.y,
            (size_t)in_size.x
        };

#define _FOO
#ifdef _FOO
        for(int y=0; y < in_size.y; y++) {
            for(int x=0; x < in_size.x; x++) {
                int ix0 = x + y * in_size.x;
#else
                for(size_t ix0=0; ix0 < _len; ix0++) {
#endif

                int self = in_map[ix0];
                int i_self = _tile_to_index[self];
                
                for(size_t z=0; z < kNumNeighbors; z++) {
                    const int2& n = _neighbors[z];
#ifdef _FOO
                    int2 p = {
                        (x + n.x), 
                        (y + n.y) 
                    };
#else
                    auto coord = delinearize(ix0, in_shape);
                    int2 p = {
                        ((int)coord[1] + n.x), 
                        ((int)coord[0] + n.y) 
                    };

#endif
                    if(is_valid(p, in_size)) {
                        int other = in_map[p.x + p.y * in_size.x];
                        int i_other = _tile_to_index[other];
                        res[i_self * kNumNeighbors + z] |= _bitmask_of(i_other);
                    }
                }
#ifdef _FOO
            }
#endif
        }
    }

    void build_weights(const T* in_map, const int2& in_size, [[maybe_unused]] decltype(_weights)& weights) {
        size_t n = in_size.x * in_size.y;
        std::multiset<T> temp(in_map, in_map + n);
        for(auto it = temp.begin(); it != temp.end(); it = temp.upper_bound(*it)) {
            size_t tile_i = _tile_to_index[*it];
            weights[tile_i] = temp.count(*it);//1.0f - 1.0f / temp.count(*it);
        }
    }


public:

    model(const T* in_map, const int2& in_size, T* , const int2& out_size) 
        : _size(out_size) {

        _len = _size.x * _size.y;

        _output_shape = {
            (size_t)out_size.y,
            (size_t)out_size.x
        };

        size_t n = in_size.x * in_size.y;
        std::vector<T> tileset(in_map, in_map + n);
        std::sort(tileset.begin(), tileset.end());
        tileset.erase(std::unique(tileset.begin(), tileset.end()), tileset.end());
    
        for(size_t i=0; i < tileset.size(); i++) {
            _tile_to_index[tileset[i]] = i;
            _index_to_tile[i] = tileset[i];
        }

        DEBUG_VALUE_OF(build_neighbors());
        
        _adjacency = decltype(_adjacency)(tileset.size() * kNumNeighbors, 0);
        build_adjacency(in_map, in_size, _adjacency);
        DEBUG_VALUE_OF(_adjacency);

        _weights = decltype(_weights)(tileset.size());
        build_weights(in_map, in_size, _weights);
        DEBUG_VALUE_OF(_weights);
        
        _coefficients = decltype(_coefficients)(_size.x * _size.y, 0);

        reset();
    }

    void reset() {
        DEBUG_METHOD();
        bitmask_t initial_mask = 0;
        for(auto it : _tile_to_index) {
            initial_mask |= _bitmask_of(it.second);
        }

        std::fill(_coefficients.begin(), _coefficients.end(), initial_mask);
    }
    
    void step() {
        //int2 coord = { 1, 1 } ;
        size_t index;
        /**
         * step 1: find the coordinate of the coefficient with the lowest entropy
         */
        get_min_entropy(&index);
        /**
         * step 2: collapse the wave function for the found coordinate
         */
        collapse(index);
        /**
         * step 3: propagate the effects of the collapse
         */
        propagate(index);
    }

    void run(T* out_map) {
        while(!is_fully_collapsed()) {
            step();
        }
        std::vector<T> temp(_size.x * _size.y, -1);

        for(int y: range(_size.y)) {
            for(int x: range(_size.x)) {
                int i = x + y * _size.x;
                bitmask_t cb = _coefficients[i];
                int index = _index_of(cb);
                temp[i] = _index_to_tile[index];

            }
        }
        std::copy(std::begin(temp), std::end(temp), out_map);
    }

};

namespace _wfc {
    template <typename T>
    static void _run(const T* in_map, const int2& in_size, T* out_map, const int2& out_size) {

#if 1
        model<T>* _ = new model<T>(in_map, in_size, out_map, out_size);
        return _->run(out_map);
#else
        return wfc(in_map, in_size, out_map, out_size);
#endif
    }
}
