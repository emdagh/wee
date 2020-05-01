#pragma once

#include <hokusai/rotation.hpp>
#include <hokusai/tileset.hpp>
#include <hokusai/util.hpp>
#include <nlohmann/json_fwd.hpp>

#define USE_VECTOR


template <typename T, size_t N>
struct adjacency_list {
    typedef typename topology<N>::value_type shape_type;
    
    static const size_t kNumNeighbors = N << 1;

#ifdef  USE_VECTOR
    std::vector<T> _data;
#else
    std::unordered_map<T, T> _data;
#endif

    adjacency_list(size_t n) {
#ifdef USE_VECTOR
        _data.resize(n * kNumNeighbors);
#endif
    }

    bool has(size_t a, size_t d) const {
#ifdef USE_VECTOR
        return _data[index_for_neighbor(a, d)] > 0;
#else
        return _data.count(index_for_neighbor(a, d)) > 0;
#endif
    }
    
    void add(size_t i_a, size_t i_b, size_t d, bool do_inverse = false) {
        wee::push_bits(_data[index_for_neighbor(i_a, d)], static_cast<T>(to_bitmask(i_b)));
    }

    T at(size_t i, size_t d) const  {
        return _data.at(index_for_neighbor(i, d));
    }

    size_t index_for_neighbor(size_t i, size_t d, tile_rotation r = tile_rotation::identity) const {

        [[maybe_unused]] constexpr size_t kMaxTileRotation = static_cast<size_t>(tile_rotation::max_tile_rotation);
#if 0
        ndindexer<2> ix({
            (ptrdiff_t)_data.size(),
            (ptrdiff_t)kNumNeighbors 
        });
        return ix.linearize(i, d);
#else
        return i * kNumNeighbors + d;
#endif
    }

    template<typename InputIt>
    void add_example(InputIt first, const tileset<T>& ts, const topology<N>& topo) {
        
        size_t n = array_product(topo.shape());

        for(auto idx : range(n)) {
            for(auto d: range(kNumNeighbors)) {
                size_t j;
                if(topo.try_move(idx, d, &j)) {
                    add(
                        ts.to_index(first[idx]), 
                        ts.to_index(first[j]), d, false
                    );
                }
            }
        }
    }
    const T& operator [] (size_t i) const { return _data.at(i); }
};

#if 0

template <typename T, size_t N, typename std::enable_if<N==3, void>::type* = nullptr>
void to_json(json& j, const adjacency_list<T,N>& a) {
    std::unordered_map<size_t, std::string> direction_to_string = {
        { 0, "xmax" },
        { 1, "ymax" },
        { 2, "zmax" },
        { 3, "xmin" },
        { 4, "ymin" },
        { 5, "zmin" }
    };

    for(const auto& [key, val] : a._data) {
        
    }
}

template <typename T, size_t N, typename std::enable_if<N==2, void>::type* = nullptr>
void to_json(json& j, const adjacency_list<T,N>& a) {
#if 1
    [[maybe_unused]]constexpr static const size_t kNumNeighbors = N << 1;
    std::unordered_map<size_t, std::string> direction_to_string = {
        { 0, "xmax" },
        { 1, "ymax" },
        { 2, "xmin" },
        { 3, "ymin" }
    };
#endif
}

template <typename T, size_t N, typename std::enable_if<N==2, void>::type* = nullptr>
void from_json(const json& j, const adjacency_list<T,N>& a) {
    /**
     * "adjacencies" : [ {"left" : ["ground"], "right" : ["ground"]}]
     */
    std::unordered_map<std::string, size_t> string_to_direction;
    for(const auto& a: j["adjacencies"]) {

    }
}
#endif