#pragma once


template <typename T, size_t N>
struct wave_propagator {
    static const size_t kNumNeighbors = N << 1;

    typedef std::function<void(const wave_propagator<T,N>&, size_t)> callback_type;
    typedef typename topology<N>::value_type coordinate_type;

    wave<T>* _wave = nullptr;
    topology<N> _topo;
    //wee::random _rnd;
    callback_type on_update;

    explicit wave_propagator(wave<T>* w, const topology<N>& topo) : _wave(w), _topo(topo) {
    }

    const topology<N>& topo() const { return _topo; }

    void step(const std::vector<float> weights, const adjacency_list<T,N>& adj) {
        size_t i = _wave->collapse(weights);
        on_update(*this, i);
        propagate(i, adj);
    }

    void propagate(size_t at, const adjacency_list<T, N>& adj) const {
        std::vector<T> open = { at };
        while(!open.empty()) {
            T self = open.back();
            open.pop_back();
            for(auto d: range(kNumNeighbors)) {
                size_t other;
                if(!_topo.try_move(self, d, &other)) {
                    continue;
                }
                T m = 0;
                for(auto a : _wave->avail_at(self)) {
                    if(adj.has(a, d)) {
                        wee::push_bits(m, adj.at(a, d));
                    }
                }
                if(m == 0) {
                    continue;
                }
                auto any = _wave->any_possible(other, m);
                if(!any) {
                    DEBUG_LOG("no options left for neighbor tile");
                    continue; // <-- this appears to give the best results.
                }
                if(!_wave->is_same(other, any)) {
                   open.push_back(other);
                }
                _wave->collapse_at(other, any);
                if(_wave->is_collapsed_at(other)) {
                    on_update(*this, other);
                }
            }
        }
    }

    const T& data(size_t i) const {
        return _wave->data()[i];
    }

    void limit(size_t i, T t) const {
        _wave->collapse_at(i, t);
    }

    void pop(size_t i, T t) const {
        if(!_wave->is_collapsed_at(i)) {
            _wave->pop(i, t);
        }
    }

    bool is_done() const {
        for(auto i: range(_wave->length())) {
            if(!_wave->is_collapsed_at(i)) {
                return false;
            }
        }
        return true;
    }

    /*void run(const basic_model<T,N>* model) {
    }*/
    
};
