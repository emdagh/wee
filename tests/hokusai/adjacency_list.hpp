#pragma once

#include <hokusai/rotation.hpp>
#include <hokusai/tileset.hpp>
#include <hokusai/util.hpp>

template <typename T, size_t N>
struct adjacency_list {
    typedef typename topology<N>::value_type shape_type;
    
    static const size_t kNumNeighbors = N << 1;

    //std::vector<T> _data;
    std::unordered_map<T, T> _data;

    adjacency_list(size_t n) {
        //_data.resize(n * kNumNeighbors);
    }

    bool has(size_t a, size_t d) const {
        return _data.count(index_for_neighbor(a, d)) > 0;
    }
    
    void add(size_t i_a, size_t i_b, size_t d, bool do_inverse = false) {
        wee::push_bits(_data[index_for_neighbor(i_a, d)], to_bitmask(i_b));
    }

    T at(size_t i, size_t d) const  {
        return _data.at(index_for_neighbor(i, d));
    }

    size_t index_for_neighbor(size_t i, size_t d, tile_rotation r = tile_rotation::identity) const {

        [[maybe_unused]] constexpr size_t kMaxTileRotation = static_cast<size_t>(tile_rotation::max_tile_rotation);
//        z * height + y * width + x;
        //return i * kNumNeighbors + d * kMaxTileRotation + static_cast<size_t>(r);
        return i * kNumNeighbors + d + static_cast<size_t>(r);//+ static_cast<size_t>(r);
    }

    template<typename InputIt>
    void add_example(InputIt first, const tileset<T>& ts, const topology<N>& topo) {
        
        size_t n = array_product(topo.shape());

        ndindexer<N> ix(topo.shape());
        for(auto idx : range(n)) {
            for(auto d: range(kNumNeighbors)) {
                size_t j;
                if(topo.try_move(idx, d, &j)) {
                    add(ts.to_index(first[idx]), ts.to_index(first[j]), d, false);
                }
            }
        }
    }
    const T& operator [] (size_t i) const { return _data.at(i); }
};
