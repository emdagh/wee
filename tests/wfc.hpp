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

using wee::range;

struct int2 {
    int x, y;
};

std::ostream& operator << (std::ostream& os, const int2& i) {
    return os << "{ 'x' : " << i.x << ", 'y' : " << i.y << "}";
}

class model {
    static constexpr size_t kNumDimensions = 2;
    static constexpr size_t kNumNeighbors = kNumDimensions * 2;
    typedef uint64_t bitmask_t;

    std::vector<bitmask_t>  _coefficients;  // bitmask of possible tile values
    std::vector<float>      _weights;       // maps tile index to weight.
    std::vector<size_t>     _adjacency;
    std::unordered_map<int, int> _tile_to_index;
    std::unordered_map<int, int> _index_to_tile;
    int2 _size;

    size_t _index_of(const bitmask_t& m) {
        return __builtin_ctzll(m); // hard-coded for 64 bit at the moment
    }

    bitmask_t _bitmask_of(size_t i) {
        return 1ULL << i;
    }

    std::vector<size_t> _avail(const bitmask_t& val) { //const int2& at) {
        //size_t index = at.x + at.y * _size.x;
        //auto tmp = _coefficients[index];
        auto tmp = val;
        
        std::vector<size_t> opts(__popcount(tmp));

        for(auto i: range(opts.size())) {
            opts[i] = _index_of(tmp);
            auto lb = tmp & -tmp;
            tmp ^= lb;
        }
        return opts;
    }

    float _shannon_entropy(const int2& at) {
        float sum_of_weights = 0.0f;
        float sum_of_weight_log_weights = 0.0f;
        auto opts = _avail(_coefficients[at.x + at.y * _size.x]);
        for(auto i: opts) {
            float w = _weights[i];
            sum_of_weights += w;
            sum_of_weight_log_weights += w * std::log(w);
        }
        return std::log(sum_of_weights) - (sum_of_weight_log_weights / sum_of_weights);
    }

    void get_min_entropy(int2* d) {
        float min_entropy = std::numeric_limits<float>::infinity();
        for(auto y: range(_size.y)) {
            for(auto x: range(_size.x)) {
                size_t i = x + y * _size.x;

                if(__popcount(_coefficients[i]) == 1) continue;

                float entropy = _shannon_entropy({x, y}) - wee::randf() / 1000.0f;
                if(entropy < min_entropy) {
                    min_entropy = entropy;
                    d->x = x;
                    d->y = y;
                }
            }
        }
    }

    void constrain(const int2& at, size_t forbidden) {
        size_t i = at.x + at.y * _size.x;
        _coefficients[i] &= ~_bitmask_of(forbidden);
    }

    void collapse(const int2& at) {
        //_coefficients[at.x + at.y * _size.x] = 2;
        auto i = at.x + at.y * _size.x;
        std::map<int, float> w; 
        float total_weight = 0.0f;
        for(auto t: _avail(_coefficients[at.x + at.y * _size.x])) {
            w.insert(std::pair(t, _weights[t]));
            total_weight += _weights[t];
        }

        float random = wee::randf(0, total_weight);

        for(const auto& [key, val]: w) {
            random -= val;
            if(random < 0) {
                _coefficients[i] = _bitmask_of(key);
                break;
            }
        }
        debug_matrix(_coefficients, _size);
    }

    void propagate(const int2& at) {
        static constexpr int2 neighbors[kNumNeighbors] = {
            { 1, 0 },
            { 0, 1 },
            {-1, 0 },
            { 0,-1 }
        };
        std::vector<int2> open = { at };
        while(!open.empty()) {
            const int2& cur_coords = open.back();
            //size_t cur_i = cur_coords.x + cur_coords.y * _size.x;
            auto current_opts = _avail(_coefficients[cur_coords.x + cur_coords.y * _size.x]);
            open.pop_back();

            for(size_t i=0; i < kNumNeighbors; i++) { //const int2& n: neighbors) {
                const int2& d = neighbors[i];
                int2 other_coords = {
                    (cur_coords.x + d.x + _size.x) % _size.x,
                    (cur_coords.y + d.y + _size.y) % _size.y
                };


                size_t other_i = other_coords.x + other_coords.y * _size.x;
                //bitmask_t other_tile = _coefficients[other_i];


                //if(__popcount(other_tile) == 1) continue;


                for(auto opt: current_opts) {
                    _coefficients[other_i] &= _adjacency[opt * 4 + i];//_bitmask_of(opt);
                    /*auto is_possible = _adjacency[opt * 4 + i] & other_tile;

                    if(!is_possible) {
                        auto cur_mask = _bitmask_of(opt);
                        _coefficients[other_i] &= cur_mask;
                        if(_coefficients[other_i] == 0) {
                            [[maybe_unused]] int k = 0;
                        }
                        open.push_back(other_coords);
                    }*/
                }
                if(__popcount(_coefficients[other_i]) > 1)
                    open.push_back(other_coords);
            }
        }
    }

    
   
    bool is_fully_collapsed() {
        for(auto i: _coefficients) {
            if(__popcount(i) > 1) return false;
        }
        return true;
    }

    void build_adjacency(const int* in_map, const int2& in_size, decltype(_adjacency)& res) {        
        constexpr int2 neighbors[kNumNeighbors] = { 
            { 0,  1}, // top    
            { 1,  0}, // right
            { 0, -1}, // bottom
            {-1,  0}, // left
        };
        for(int y=0; y < in_size.y; y++) {
            for(int x=0; x < in_size.x; x++) {
                int ix0 = x + y * in_size.x;
                
                int self = in_map[ix0];
                int i_self = _tile_to_index[self];

                for(size_t z=0; z < kNumNeighbors; z++) {
                    const int2& n = neighbors[z];
                    int2 p = {
                        (x + n.x + in_size.x) % in_size.x, // periodicity
                        (y + n.y + in_size.y) % in_size.y 
                    };

                    //if( p.x > 0 && p.x < (in_size.x - 1) && 
                    //    p.y > 0 && p.y < (in_size.y - 1)) {
                        int nt = in_map[p.x + p.y * in_size.x];
                        int i_other = _tile_to_index[nt];

                        res[i_self * 4 + z] |= _bitmask_of(i_other);
                    //}
                }
            }
        }
    }

    void build_weights(const int* in_map, const int2& in_size, [[maybe_unused]] decltype(_weights)& weights) {
        size_t n = in_size.x * in_size.y;
        std::multiset<int> temp(in_map, in_map + n);
        for(auto it = temp.begin(); it != temp.end(); it = temp.upper_bound(*it)) {
            size_t tile_i = _tile_to_index[*it];
            weights[tile_i] = temp.count(*it);//1.0f - 1.0f / temp.count(*it);
        }




        /*std::map<size_t, float> temp;
        for(auto y: range(in_size.y)) {
            for(auto x: range(in_size.x)) {
                size_t self_i = _tile_to_index(in_map[x + y * in_size.x]);

                if(temp.count(self_i)) {
                    temp[self_i] = 0.0f;
                }
                temp[self_i] += 1.0f;
            }
        }*/
    }

    template <typename T>
    void debug_matrix(const std::vector<T>& a, const int2& s) {
        for(auto y: range(s.y)) {
            auto begin = a.begin() + y * s.x;
            auto end = begin + s.x;
            auto row = std::vector<T>(begin, end);
            DEBUG_VALUE_OF(row);
        }
        std::cout << std::endl;
    }

public:

    model(const int* in_map, const int2& in_size, int* , const int2& out_size) 
        : _size(out_size) {

            auto x = _avail(1);
        int n = in_size.x * in_size.y;
        std::vector<int> tileset(in_map, in_map + n);
        std::sort(tileset.begin(), tileset.end());
        tileset.erase(std::unique(tileset.begin(), tileset.end()), tileset.end());
    
        for(size_t i=0; i < tileset.size(); i++) {
            _tile_to_index[tileset[i]] = i;
            _index_to_tile[i] = tileset[i];
        }

        _adjacency = decltype(_adjacency)(tileset.size() * kNumNeighbors, 0);
        build_adjacency(in_map, in_size, _adjacency);
        DEBUG_VALUE_OF(_adjacency);

        _weights = decltype(_weights)(tileset.size());
        build_weights(in_map, in_size, _weights);
        DEBUG_VALUE_OF(_weights);

        bitmask_t initial_mask = 0;
        for(auto it : _tile_to_index) {
            initial_mask |= _bitmask_of(it.second);
        }

        _coefficients = decltype(_coefficients)(out_size.x * out_size.y, initial_mask);
        debug_matrix(_coefficients, out_size);
    }
    
    void step() {
        int2 coord;
        /**
         * step 1: find the coordinate of the coefficient with the lowest entropy
         */
        get_min_entropy(&coord);
        /**
         * step 2: collapse the wave function for the found coordinate
         */
        collapse(coord);
        /**
         * step 3: propagate the effects of the collapse
         */
        propagate(coord);
    }

    void run(int* out_map) {
        while(!is_fully_collapsed()) {
            step();
        }
        std::vector<int> temp(_size.x * _size.y, -1);

        for(int y: range(_size.y)) {
            for(int x: range(_size.x)) {
                int i = x + y * _size.x;
                bitmask_t cb = _coefficients[i];
                int index = _index_of(cb);
                temp[i] = _index_to_tile[index];

            }
        }
        debug_matrix(temp, _size);
        std::copy(std::begin(temp), std::end(temp), out_map);
    }

};

namespace wfc {
    static void _run(const int* in_map, const int2& in_size, 
        int* out_map, const int2& out_size) {

#if 1
        model* _ = new model(in_map, in_size, out_map, out_size);
        return _->run(out_map);
#endif
    }
}


/*namespace _wfc {

    auto as_bitmask(size_t a) { return 1ULL << a; }
    auto as_index(const uint64_t& a) { return __builtin_ctzll(b); } 


    namespace detail {
        template <typename T>
        bool pop_vector(std::vector<T>& a, T* d) {
            if(a.empty()) return;
            *d = a.back();
            a.pop_back();
            return !a.empty();
        }
    }

    struct model {
        typedef uint64_t bitmask_t;

        const int* in_map;
        int2 in_size;
        int* out_map;
        int2 out_size;

        std::vector<bitmask_t> coeff;
        std::vector<float> weights;

        model(const int* in_map, const int2& in_size, int* out_map, const int2& out_size) 
            : in_map(in_map)
            , in_size(in_size)
            , out_map(out_map)
            , out_size(out_size)
        {
            //cell = new bitmask_t[out_size.x * out_size.y];
            //memset(cell, -1, sizeof(bitmask_t) * out_size.x * out_size.y);
            size_t n = out_size.x * out_size.y;
            coeff = std::vector<bitmask_t>(n, -1);
        }

        float shannon_entropy(size_t i) {
            for(auto opt: avail(coeff[i])) {
                float H = 1.0f - (1.0f / (float)popcount(opt));

            }
        }

        bool is_collapsed() {
            for(auto i: coeff) {
                if(popcount(i) > 1) return false;
            }
        }

        void find_min_entropy(int2* ret) {
            float min_entropy = 0.0f;
            int2 min_entropy_coords;
            size_t n = out_size.x * out_size.y;
            for(auto y: range(out_size.y)) {
                for(auto x: range(out_size.x)) {
                    auto i = x + y * out_size.x;

                    if(popcount(coeff[i]) == 1) 
                        continue;

                    float entropy = shannon_entropy(i) - wee::randf(0, 0.001f);
                    if(entropy < min_entropy) {
                        min_entropy = entropy;
                        *ret = { x, y };
                    }
                }
            }
        }

        void collapse(const int2& at) {
            auto i = at.x + at.y * out_size.x;
            bitmask_t mask = coeff[i];
            std::map<int, float> w; 
            float total_weight = 0.0f;
            for(auto t: avail(at)) {
                w.insert(t, weights[t]);
                total_weight += weights[t];
            }

            float random = randf(0, total_weight);
            for(const auto& [key, val]: w) {
                random -= val;
                if(random < 0) {
                    coeff[i] = key;
                }
            }

        }

        void step() {
            int2 coord = find_min_entropy();
            collapse(coord);
            propagate(coord);
        }


        std::vector<int> avail(const int2& coord) {
            size_t index = coord.x + coord.y * out_size.x;
            auto tmp = coeff[index];
            std::vector<int> opts(popcount(tmp));
            for(auto i: range(opts.size())) {
                opts[i] = to_index(tmp);
                auto lb = tmp & -tmp;
                tmp ^= lb;
            }
            return opts;
        }

        void propagate(const int2& coord) {
            static constexpr int2 neighbors[] = {
                { 0, 1 },
                { 1, 0 },
                { 0,-1 },
                {-1, 0 }
            };

            std::vector<int2> open = { coord };
            int2 current_coord;
            do {
                _pop(open, &current_coord);
                auto options = avail(current_coord);
                for(auto d: neighbors) {
                    auto neigbor_coord = {
                        (current_coord.x + d.x + out_size.x) % out_size.x,
                        (current_coord.y + d.y + out_size.y) % out_size.y
                    };
                    auto is_possible = avail(current_coord) & avail(neighbor_coord);

                    if(!is_possible) {
                        cell[neighbor_coord] &= neighbor;
                    }
                }
            } while(!open.empty());
        }
    };
}
*/
