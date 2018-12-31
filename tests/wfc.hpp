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
#include <nmmintrin.h>

#define RANDOM_SEED     0

using wee::range;

#define __POPCOUNT(x)   __builtin_popcountll(x)

template <typename T, size_t kNumDimensions = 2>
class alignas(128) model {

    size_t _len;
    wee::random _random;// = { 60413549 };// { 25298873 };// { 4025143874 };// { -279628382}; 

    static constexpr size_t kNumNeighbors = kNumDimensions * 2;
    typedef uint64_t bitmask_t;//uint64_t bitmask_t;

    //union {
        std::vector<float>         _weights;       // maps tile index to weight.
    //    __m128* _mm_weights;
    //};

    //union {
        std::vector<bitmask_t>     _coefficients;  // bitmask of possible tile values
    //    __m128i* _mm_coefficients;
    //};

    //union {
        std::vector<bitmask_t>     _adjacency;
    //    __m128i* _mm_adjacency;
    //};

    std::unordered_map<T, int> _tile_to_index;
    std::unordered_map<int, T> _index_to_tile;
    std::valarray<size_t>      _output_shape;
    std::valarray<int>         _neighbors;


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


    float _shannon_entropy(size_t at_i) { 
        //auto at_i = at.x + at.y * _size.x;
#if 0 
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
        auto opts = avail(_coefficients[at_i]);
        for(auto i: opts) {
            float w = _weights[i];
            sum_of_weights += w;
            sum_of_weight_log_weights += w * std::log(w);
        }
        return std::log(sum_of_weights) - (sum_of_weight_log_weights / sum_of_weights);
#else // this version is ~ 3x faster
        return 1.0f - 1.0f / static_cast<float>(__POPCOUNT(_coefficients[at_i]));
#endif
    }


    void get_min_entropy(size_t* k) { //std::valarray<size_t>* d) {
        float min_H = std::numeric_limits<float>::infinity();

        for(size_t i=0; i < _len; i++) {
            if(__POPCOUNT(_coefficients[i]) == 1)
                continue;

            float H = _shannon_entropy(i) - _random.next<float>(0.f, 1.f) / 1000.0f;
            if(H < min_H) {
                min_H = H;
                *k = i;
            }
        }
    }

    void collapse(size_t i) { 
        //auto i = at.x + at.y * _size.x;
        
        std::map<int, float> w; 
        float total_weight = 0.0f;
        auto avail_at = avail(_coefficients[i]);
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



    bool is_valid(const std::valarray<int>& coord, const std::valarray<size_t>& shape) {
        size_t n = shape.size();
        assert(n == coord.size());
        for(auto i: wee::range(n)) {
            if(coord[i] < 0 || shape[i] <= static_cast<size_t>(coord[i]))
                return false;
        }
        return true;
    }
    
    void propagate(size_t at) { 
        std::vector<size_t> open = { at };

        while(!open.empty()) {
            size_t cur_i = open.back();
            open.pop_back();
            auto cur_coords = delinearize<int>(cur_i, _output_shape);
            bitmask_t cur_bitmask = _coefficients[cur_i];
            auto cur_avail = avail(cur_bitmask);

            for(size_t i=0; i < kNumNeighbors; i++) { 
                size_t start = i * kNumDimensions;
                std::valarray<int> d = _neighbors[std::slice(start, kNumDimensions, 1)];

                auto other_coords = cur_coords + d;

                if(!is_valid(other_coords, _output_shape)) 
                    continue;

                size_t other_i = linearize(other_coords, _output_shape);

                bitmask_t opts = 0;
                for(auto ct: cur_avail) {
                    opts |= _adjacency[ct * kNumNeighbors + i];
                }
                // _mm_and_si128(_mm_coefficients[...], _mm_opts)
                
                /**
                 * this next line is important! This prevents the selection of tiles that have 0 neighbors 
                 */
                if(!opts) 
                    continue;
                
                auto all_options = _coefficients[other_i] & opts;
                if(!all_options) {
                    return;
                }
                
                if(_coefficients[other_i] != all_options) {
                    open.push_back(other_i);//coords);
                }
                _coefficients[other_i] = all_options;
            }
        }
    }
   
    bool is_fully_collapsed() {
        for(auto i: _coefficients) {
            if(__POPCOUNT(i) > 1) return false;
        }
        return true;
    }

    void build_adjacency(const T* in_map, const std::valarray<size_t>& in_shape, decltype(_adjacency)& res) {
        std::fill(res.begin(), res.end(), 0);

        size_t in_len = array_product(in_shape);

        for(size_t ix0=0; ix0 < in_len; ix0++) {

            int self = in_map[ix0];
            int i_self = _tile_to_index[self];
            
            auto coord = delinearize<int>(ix0, in_shape);
            
            for(size_t z=0; z < kNumNeighbors; z++) {
                size_t start = z * kNumDimensions;
                std::valarray<int> n = _neighbors[std::slice(start, kNumDimensions, 1)];
                std::valarray<int> p = coord + n;
                
                if(is_valid(p, in_shape)) {
                    int other = in_map[linearize(p, in_shape)];
                    int i_other = _tile_to_index[other];
                    res[i_self * kNumNeighbors + z] |= _bitmask_of(i_other);
                }
            }
        }
    }

    void build_weights(const T* in_map, size_t n, decltype(_weights)& weights) {
        std::multiset<T> temp(in_map, in_map + n);
        for(auto it = temp.begin(); it != temp.end(); it = temp.upper_bound(*it)) {
            size_t tile_i = _tile_to_index[*it];
            weights[tile_i] = temp.count(*it);//1.0f - 1.0f / temp.count(*it);
        }
    }

    template <typename S>
    size_t array_product(const std::valarray<S>& a) {
        return std::accumulate(std::begin(a), std::end(a), 1, std::multiplies<T>());
    }
public:

    model(const T* in_map, const std::valarray<size_t>& in_size, T* , const std::valarray<size_t>& out_size) 
    {
        assert(in_size.size() == out_size.size());
        _len = array_product(out_size);

        _output_shape = out_size;


        size_t n = array_product(in_size);
        std::vector<T> tileset(in_map, in_map + n);
        std::sort(tileset.begin(), tileset.end());
        tileset.erase(std::unique(tileset.begin(), tileset.end()), tileset.end());
        DEBUG_VALUE_OF(tileset);
    
        for(size_t i=0; i < tileset.size(); i++) {
            _tile_to_index[tileset[i]] = i;
            _index_to_tile[i] = tileset[i];
        }

        _neighbors = build_neighbors();
        DEBUG_VALUE_OF(_neighbors);
        
        _adjacency = decltype(_adjacency)(tileset.size() * kNumNeighbors, 0);
        build_adjacency(in_map, in_size, _adjacency);
        DEBUG_VALUE_OF(_adjacency);

        _weights = decltype(_weights)(tileset.size());
        build_weights(in_map, n, _weights);
        DEBUG_VALUE_OF(_weights);
        
        _coefficients = decltype(_coefficients)(_len, 0);

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

    

    void coeff(T* out_map) {
        std::copy(std::begin(_coefficients), std::end(_coefficients), out_map);
    }

    void gather(T* out_map) {
        std::vector<T> temp(_len, -1);
        for(size_t i=0; i < _len; i++) {
                //int i = x + y * _size.x;
                bitmask_t cb = _coefficients[i];
                int index = _index_of(cb);
                temp[i] = _index_to_tile[index];
        }
        std::copy(std::begin(temp), std::end(temp), out_map);
    }

    void run(T* out_map) {
        while(!is_fully_collapsed()) {
            step();
        }
        gather();
        DEBUG_VALUE_OF(_random.seed());
    }
    std::vector<size_t> avail(const bitmask_t& val) { 
        auto tmp = val;
        
        std::vector<size_t> opts(__POPCOUNT(tmp));

        for(auto i: range(opts.size())) {
            opts[i] = _index_of(tmp);
            auto lb = tmp & -tmp;
            tmp ^= lb;
        }
        return opts;
    }

    T tile(const size_t& index) {
        //assert(__POPCOUNT(bm) == 1);
        return _index_to_tile[index];
    }

};

namespace _wfc {
    template <typename T>
    static void _run(const T* in_map, const int2& in_size, T* out_map, const int2& out_size) {

#if 1
        std::valarray<size_t> in_shape = {
            (size_t)in_size.y,
            (size_t)in_size.x
        };

        std::valarray<size_t> out_shape = {
            (size_t)out_size.y,
            (size_t)out_size.x
        };
        
        model<T>* _ = new model<T>(in_map, in_shape, out_map, out_shape);
        return _->run(out_map);
#else
        return wfc(in_map, in_size, out_map, out_size);
#endif
    }
}
