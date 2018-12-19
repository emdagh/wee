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
struct int2 {
    int x, y;
};

std::ostream& operator << (std::ostream& os, const int2& i) {
    return os << "{ 'x' : " << i.x << ", 'y' : " << i.y << "}";
}

class model {
    typedef uint64_t bitmask_t;

    std::vector<bitmask_t>  _coefficients;  // bitmask of possible tile values
    std::vector<float>      _weights;       // maps tile index to weight.
    int2 _size;

    size_t _index_of(const bitmask_t& m) {
        return __builtin_ctzll(m); // hard-coded for 64 bit at the moment
    }

    bitmask_t _bitmask_of(size_t i) {
        return 1ULL << i;
    }

    std::vector<size_t> _avail(const int2& at) {
        size_t index = at.x + at.y * _size.x;
        auto tmp = _coefficients[index];
        
        std::vector<int> opts(popcount(tmp));

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
        for(auto i: _avail(at)) {
            float w = _weights[i];
            sum_of_weights += w;
            sum_of_weight_log_weights += w * std::log(w);
        }
        return std::log(sum_of_weights) - (sum_of_weight_log_weights / sum_of_weights);
    }

    void get_min_entropy(int2* d) {
        float min_entropy = 0.0f;
        for(auto y: range(_size.y)) {
            for(auto x: range(_size.x)) {
                if(popcount(_coefficients]) == 1) continue;

                float entropy = _shannon_entropy({x, y}) - randf() / 1000.0f;
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

    void propagate(const int2& at) {
        static constexpr size_t kNumEdges = 4;
        static constexpr int2 neighbors[] = {
            { 1, 0 },
            { 0, 1 },
            {-1, 0 },
            { 0,-1 }
        };
        std::stack<int2> open = { at };
        while(!open.empty()) {
            const int2& cur_coords = open.top();
            size_t cur_i = cur_coords.x + cur_coords.y * _size.x;
            open.pop();
            for(size_t i=0; i < kNumEdges; i++) { //const int2& n: neighbors) {
                const int2& d = neighbors[i];
                int2 other_coords = {
                    (cur_coords.x + d.x + _size.x) % _size.x,
                    (cur_coords.y + d.y + _size.y) % _size.y
                };

                size_t other_i = other_coords.x + other_coords.y * _size.x;
                bitmask_t other_tile = _coefficients[other_i];

                auto is_possible = _adjacency[cur_index * 4 + i] & other_tile;

                if(!is_possible) {
                    constrain(other_coords, other_tile);
                    open.push(other_coords);
                }
            }
        }
    }

    
   
    bool is_fully_collapsed() {
        for(auto i: _coefficients) {
            if(popcount(i) > 1) return false;
        }
        return true;
    }

public:

    model() {}

    
    void step() {
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

    void run() {
        while(!is_fully_collapsed) {
            step();
        }
    }

};


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
