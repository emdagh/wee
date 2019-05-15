#pragma once 

#include <vector>
#include <core/ndarray.hpp>
#include <core/inverse_map.hpp>
#include <core/bits.hpp>
/**
 *
 * TODO: create adjecancy list model
 */

namespace nami {
    using wee::range;
    using wee::linearize;
    using wee::delinearize;
    using wee::inverse_map;
    using wee::ctz;
    using wee::popcount;




    template <size_t kNumNeighbors, size_t kNumDimensions>
    std::valarray<int> build_neighbors() {
        static const constexpr size_t kN = kNumNeighbors * kNumNeighbors;
        static const constexpr size_t kH = kN / 2;
        std::valarray<int> n(0, kN);
        n[std::gslice(0, { kNumNeighbors / 2, kNumDimensions }, { kNumDimensions + 1, 0 })] = 1; // upper left quadrant identity trace

        n[std::gslice(kH, { kNumNeighbors /2, kNumDimensions }, { kNumDimensions + 1, 0})] = -1; // lower right quadrant inverse identity trace
        return n;
    } 
    
    template <typename T, typename M = std::size_t, size_t kNumDimensions = 2>
    class model { 
    
        template <typename S>
        size_t array_product(const S& t) {
            return std::accumulate(
                std::begin(t), 
                std::end(t), 
                1, 
                std::multiplies<typename S::value_type>()
            );
        }
        static_assert(std::is_integral<T>::value);
        static const constexpr size_t kNumNeighbors = kNumDimensions << 1;
        //typedef uint64_t bitmask_type;
        typedef M bitmask_type;
        typedef std::vector<bitmask_type> adjacency_list_type;

        adjacency_list_type _adjacency;
        wee::random _randgen;
        std::vector<float> _weights;
        std::vector<float> _coefficients;
        std::valarray<int> _shape;
        std::valarray<int> _neighbors;
        std::unordered_map<int, size_t> _tile_to_index;
        bitmask_type _banned;
    protected:

        constexpr size_t _index_of(const bitmask_type& b) const {
            return ctz(b);
        }

        constexpr bitmask_type _bitmask_of(int i) {
            return static_cast<bitmask_type>(1) << i;
        }

        float _entropy_of(bitmask_type b) {
            return 1.0f - 1.0f / static_cast<float>(popcount(b));
        }

        void _get_min_entropy_index(size_t* ret) {
            //size_t ret = 0;
            float min_h = std::numeric_limits<float>::infinity();
            for(size_t i: range(_coefficients)) {
                const auto& coef = _coefficients[i];

                if(popcount(coef) == 1)
                    continue;

                float h = _entropy_of(coef) - _randgen.next<float>(0.0f, 1.0f) / 1000.0f;
                if(h < min_h) {
                    min_h = h;
                    *ret = i;
                }
            }
        }

        std::vector<size_t> _avail(const bitmask_type& b) {
            std::vector<size_t> options(popcount(b));
            auto tmp = b;
            for(auto i: range(options.size())) {
                options[i] = _index_of(tmp);
                auto lb = tmp & -tmp;
                tmp ^= lb;
            }
            return options;
        }

        int _collapse(size_t i) {
            std::map<int, float> w;
            float total_weight = 0.0f;
            auto avail_at = _avail(_coefficients[i]);
            for(auto t: avail_at) {
                w.insert({t, _weights[t]});
                total_weight += _weights[t];
            }

            auto random = _randgen.next<float>(0.0f, 1.0f) * total_weight;

            for(const auto& [key, val] : w) {
                random -= val;
                if(random < 0) {
                    return _coefficients[i] = _bitmask_of(key);
                }
            }
            return -1;
        }

        template <typename S>
        inline S pop (std::vector<S>& v) {
            S res = v.back();
            v.pop_back();
            return res;
        }

        bool _is_valid(const std::valarray<int>& coord, const std::valarray<int>& shape) {
            //if(_is_periodic) 
            //    return true;
            size_t n = shape.size();
            assert(n == coord.size());
            for(auto i: wee::range(n)) {
                if(coord[i] < 0 || shape[i] <= coord[i])
                    return false;
            }
            return true;
        }

        void _propagate(size_t index) {

            std::vector<size_t> open = { index };
            while(!open.empty()) {
                size_t cur_i = pop(open);
                auto cur_coords = wee::delinearize<int>(cur_i, _shape);
                bitmask_type cur_bitmask = _coefficients[cur_i];
                auto cur_avail = _avail(cur_bitmask);
                for(size_t i: range(kNumNeighbors)) {
                    constexpr size_t start = i * kNumDimensions;
                    std::valarray<int> dim = _neighbors[std::slice(start, kNumDimensions, 1)];
                    auto other_coords = cur_coords + dim;
                    if(!_is_valid(other_coords, _shape)) {
                        continue;
                    }

                    size_t other_i = wee::linearize(other_coords, _shape);
                    bitmask_type options = 0;
                    for(auto c: cur_avail) {
                        options |= _adjacency[c * kNumNeighbors + i];
                    }

                    if(!options) 
                        continue;

                    auto any_possible = _coefficients[other_i] & options;
                    if(!any_possible) {
                        return;
                    }
                    //! 
                    //! TODO: shoudn't this be '&'
                    //! as in if((_coefficients[other_i] & any_possible) != 0)
                    //! 
                    if(_coefficients[other_i] != any_possible) {
                        open.push_back(other_i);
                    }
                    _coefficients[other_i] = any_possible;
                }
            }
        }
        void _step() {
            // TODO: run callback on every step
            auto i = -1;
            _get_min_entropy_index(&i);
            _collapse(i);
            _propagate(i);
        }

        bool _fully_collapsed() const {
            for(auto i: _coefficients) {
                if(popcount(i) > 1) return false;
            }
            return true;
        }

        void _build_weights(const T* in_map, size_t n, decltype(_weights) w) {
            std::multiset<T> temp(in_map, in_map + n);
            for(auto it=temp.begin(); it != temp.end(); it = temp.upper_bound(*it)) {
                auto tile_i = _tile_to_index.at(*it);
                w[tile_i] = temp.count(*it);
            }
        }

        /**
         *! Helper function to aid in creating adjacency lists
         *! from example tile maps.
         */

        void _build_adjacency_from_example(
            const T* in_map, 
            const std::valarray<int>& in_shape,
            adjacency_list_type& res
        ) {
            std::fill(res.begin(), res.end(), 0);
            size_t in_len = array_product(in_shape);
            for(size_t ix0: range(in_len)) {
                int self = in_map[ix0];
                int self_i= _tile_to_index.at(self);
                auto coord = delinearize<int>(ix0, in_shape);

                for(size_t z: range(kNumNeighbors)) {
                    size_t start = z * kNumDimensions;
                    std::valarray<int> n = _neighbors[std::slice(start, kNumDimensions, 1)];
                    std::valarray<int> p = coord + n;
                    if(_is_valid(p, in_shape)) {
                        int other = in_map[linearize(p, in_shape)];
                        int other_i = _tile_to_index.at(other);
                        res[self_i * kNumNeighbors + z] |= _bitmask_of(other_i);
                    }
                }
            }
        }

    public:

        model(const adjacency_list_type& adj, const std::vector<float>& weights, const std::valarray<int>& in_shape, const std::valarray<int>& out_shape) {


        }

        void run(T* out) {
            while(!_fully_collapsed()) {
                _step();
            }
            size_t len = array_product(_shape);
            std::vector<T> temp(len, -1);
            for(size_t i=0; i < len; i++) {
                    temp[i] = tile(_index_of(_coefficients[i]));//_index_to_tile[index];
            }
            std::copy(std::begin(temp), std::end(temp), out);
        }

        void ban(size_t id) {
            _banned |= _bitmask_of(_tile_to_index.at(id));
        }

        const T& tile(size_t ix) {
            return inverse_map(_tile_to_index).at(ix);
        }

        static void from_example(T* in_map, const std::valarray<int>& in_shape, const std::valarray<int>& out_shape, std::unordered_map<int, size_t>& tile_to_index) {
            size_t in_len = array_product(in_shape);
            /**
             * create a tileset from the input example.
             */
            std::vector<T> tileset(in_map, in_map + in_len);
            std::sort(tileset.begin(), tileset.end());
            tileset.erase(std::unique(tileset.begin(), tileset.end()), tileset.end());
            for(size_t i: range(tileset.size())) {
                tile_to_index[tileset[i]] = i;
            }
            auto neighbors = nami::build_neighbors<kNumNeighbors, kNumDimensions>();
        }
    };
}
