#pragma once

#include <hokusai/util.hpp>
#include <hokusai/wave.hpp>
#include <hokusai/adjacency_list.hpp>
#include <hokusai/tileset.hpp>

template <typename T, size_t N>
struct basic_model {
    typedef typename topology<N>::value_type shape_type;
    typedef tileset<T> tileset_type;
    typedef adjacency_list<T,N> adjacency_list_type;
   
    std::vector<basic_constraint<T, N>* > _constraints;
    tileset_type&& _tileset;
    adjacency_list_type&& _adjacencies;
    T _banned;

    std::function<void(const wave_propagator<T,N>&)> on_update;
    

    basic_model(tileset_type&& ts, adjacency_list_type&& a) 
    : _tileset(std::forward<tileset_type>(ts))
    , _adjacencies(std::forward<adjacency_list_type>(a)) 
    {
    }

    void add_constraint(basic_constraint<T,N>* ptr) { _constraints.push_back(ptr); }

    void ban(T t) { _banned |= to_index(_tileset.to_index(t)); }

    T domain() const {
        T res = 0;
        for(auto i : range(_tileset.length())) {
            //res |= to_bitmask(_tileset.to_index(_tileset.tile(i)));
            wee::push_bits(res, static_cast<T>(to_bitmask(_tileset.to_index(_tileset.to_tile(i)))));
        }
        return res;
    }

    template <typename OutputIt>
    void solve(const typename topology<N>::value_type& shape, OutputIt d_it) { //const topology<N>& topo, OutputIt d_it) {
        auto topo = topology<N>(shape);
        auto len = topo.length();
        DEBUG_VALUE_OF(len);
        wave<T> wv(len, domain());
        /**
         * apply all constraints to the new wave
         */
        wave_propagator<T, N> prop(&wv, topo);
        /**
         * register a callback lambda that will check all constraints 
         * whenever a collapse cycle has finished.
         */
        prop.on_update = [this] (const wave_propagator<T,N>& p, size_t ix) {
            for(auto* ptr : _constraints) {
                std::vector<size_t> res;
                ptr->check(p, ix, &res);
                for(auto i: res) {
                    p.propagate(i, _adjacencies);
                }
            }
            if(this->on_update) 
                this->on_update(p);
        };
        for(auto* ptr : _constraints) {
            std::vector<size_t> res;
            ptr->init(prop, &res);
            for(auto i: res) {
                prop.propagate(i, _adjacencies);
            }
        }
        DEBUG_VALUE_OF(prop.progress());
        /**
         * here we run the wave function collapse algorithm
         */

        auto weights = _tileset.frequencies();

        while(!prop.is_done()) {
            prop.step(weights, _adjacencies);
        }
        /**
         * copy result in tile id format
         */
        std::transform(wv.data().begin(), wv.data().end(), d_it, [&] (const T& t) {
            return _tileset.to_tile(to_index(t));
        });
    }
};
