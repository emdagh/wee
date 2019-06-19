#pragma once

#include <hokusai/basic_constraint.hpp>
#include <hokusai/topology.hpp>
#include <cmath>

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

template <typename T, size_t N>
struct mirror_constraint : public basic_constraint<T,N> {

    virtual void init(const wave_propagator<T, N>&, std::vector<size_t>*) {
    }

    virtual void check(const wave_propagator<T, N>& wp, size_t i, std::vector<size_t>*) {
    }
};

template <typename T, size_t N>
struct max_consecutive_constraint : public basic_constraint<T,N> {
    T _tilemask;
    size_t _maxcount;
    std::vector<size_t> _directions;

    max_consecutive_constraint(T t, size_t maxcount, const std::vector<size_t>& directions)
    : _tilemask(t)
    , _maxcount(maxcount)
    , _directions(directions) 
    {
    }

    virtual ~max_consecutive_constraint() {
    }

    virtual void init(const wave_propagator<T, N>&, std::vector<size_t>*) {
    }

    size_t find_first(const wave_propagator<T,N>& wp, size_t i, size_t d_inv, size_t* d_first) {

        auto& topo = wp.topo();

        size_t count   = 0;
        size_t j       = 0;

        *d_first = i;


        while(topo.try_move(*d_first, d_inv, &j)) {
            if(!(wp.data(j) == _tilemask)) 
                break;
            *d_first = j;
            count++;
        }
        return count;
    }

    virtual void check(const wave_propagator<T, N>& wp, size_t i, std::vector<size_t>*) {
        /**
         * this constraint will check if the current wave has less than the 
         * maximum amount of consecutive tiles in the indicated direction.
         * It could benefit from a monitoring system.
         *
         * wp.pop(nexttile_in_current_direction, _tilemask)
         *
         * update: 2019-06-12:
         *  this will obviously not work in all cases. An edge case would be that a cell collapsed to a 
         *  tile that is in the middle of two disjoint sets of 
         *  similar tiles (low entropy is likely here).
         * 
         *  another case could be where a tile will make a run across emoty space until a disjoint set 
         *  of self similar tiles is encountered. This could be mitigated with a look-ahead of max_consucutive cells...
         */
        size_t current = i;
        auto& topo = wp.topo();

        if(wp.data(i) == _tilemask) {
#if 0
            for(auto d: _directions) {
                size_t d_min = d + N;
                size_t d_max = d;
#else
            static const size_t kNumDimensions = N;
            static const size_t kNumNeighbors = kNumDimensions << 1;

            for(auto d: _directions) {
                size_t d_min = d;
                size_t d_max = (d + kNumDimensions) % kNumNeighbors;
#endif
                size_t first;
                size_t count = find_first(wp, i, d_min, &first);
                while(topo.try_move(current, d_max, &current)) {
                    if(wp.data(current) == _tilemask) {
                        count++;
                    }
                    if(count == _maxcount) {
                        size_t k;
                        if(topo.try_move(current, d_max, &k)) {
                            wp.pop(k, _tilemask);
                        }
                    }
                }
            }
        }
    }
};

template <typename T, size_t N>
struct border_constraint : public basic_constraint<T,N> {
    T _tile;
    std::vector<size_t> _directions;

    border_constraint(T tile, const std::vector<size_t>& dir) 
    : _tile(tile)
    , _directions(dir) 
    {
    }

    virtual void init(const wave_propagator<T, N>& prop, std::vector<size_t>* res) {
        /**
         * 2019-06-01
         *
         * first of all, dermine which axis the iteration needs to be
         * processed across. This should be based on direction index
         * from topology<N>::sides.
         *
         * The mapping should look something like this:
         * [direction_index] => (axis, min/max)
         * the axis can be determined by it's index
         *      axis = index % N (where N is the number of dimensions)
         * the sign can probably be done getting the sign of the array sum of
         * the direction vector. We can multiply the result of this sum by the max
         * extent of the dimension and reach a final set of variables to into into
         * a ndindexer::slice function.
         */
        const topology<N>& topo = prop.topo();
        wee::ndindexer<N> ix(topo.shape());
        for(size_t i=0; i < _directions.size(); i++) {
            auto neighbor   = topo.neighbor(_directions[i]);
            size_t axis     = _directions[i] % N;
            
            //auto is_signed  = std::signbit(array_sum(neighbor));
            auto is_signed  = sgn(array_sum(neighbor)) < 0;
            auto slice      = is_signed * (ix.shape()[axis] - 1);
            
            ix.iterate_axis(axis, slice, [&] (auto idx) {
                prop.limit(idx, (_tile));
                res->push_back(idx);
            });
        }
    }

    virtual void check(const wave_propagator<T, N>&, size_t, std::vector<size_t>*) {
    }
};
template <typename T, size_t N>
struct corner_constraint {
    static const size_t kNumCorners = 1 << N;
    typedef typename std::array<T, kNumCorners> corners_type;
    typedef typename topology<N>::value_type shape_type;
    corners_type _corners = { 0 };

    corner_constraint(size_t axis, const corners_type& corners) { 
    }
    virtual void init(const wave_propagator<T, N>& prop, std::vector<size_t>* res) {
    }
    virtual void check(const wave_propagator<T, N>&, size_t, std::vector<size_t>*) {
    }

    static corners_type make_corners(const shape_type& shape) {
        wee::ndindexer<N> ix(shape);
        //for(auto dim : range(N)) {
        //    std::array<ptrdiff_t, N-1> aux; 
        //}
    }
        
};

template <typename T, size_t N>
struct fixed_tile_constraint : public basic_constraint<T, N> {

    typedef typename topology<N>::value_type coord_type;

    T _tilemask;
    coord_type _at;

    fixed_tile_constraint(T tilemask, const coord_type& at) : _tilemask(tilemask), _at(at) {}

    virtual void init(const wave_propagator<T, N>& prop, std::vector<size_t>* res) {
        size_t idx = prop.topo().to_index(_at);
        prop.limit(idx, _tilemask);
        res->push_back(idx);
    }
    virtual void check(const wave_propagator<T, N>&, size_t, std::vector<size_t>*) {
    }
};
