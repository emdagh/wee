#pragma once

#include <set>

template <typename T>
struct tileset {
    /**
     * TODO: can we replace all this with a single std::unordered_multiset?
     */
    std::vector<T> _data;
    std::unordered_map<T, size_t> _names;
    std::vector<float> _frequency;

    template <typename Iter>
    static tileset make_tileset(Iter first, Iter last) {
        Iter ptr = first;
        tileset res;
        while(ptr != last) {
            res.push(*ptr++);
        }
        return res;
    }

    tileset() { 
        push(0); 
    }

    T to_tile(size_t i) const { return _data[i]; }
    size_t to_index(T t) const { return _names.at(t); }//_data.at(_names.at(t)); }
    void set_frequency(T t, float f) { _frequency[to_index(t)] = f; }
    const std::vector<float> frequencies() const { return _frequency; }
    size_t length() const { return _data.size(); }
    
    template <typename InputIt, typename OutputIt>
    void make_weights(InputIt first, InputIt last, OutputIt d_first) const {
        std::multiset<typename InputIt::value_type> temp(first, last);
        for(auto it = temp.begin(); it != temp.end(); it = temp.upper_bound(*it)) {
            auto ti = to_index(*it);
            *(d_first + ti) = temp.count(*it);
        }
    }
    
    void push(T t) {
        if(_names.count(t) == 0) {
            _names.insert(std::make_pair(t, _data.size()));
            _data.push_back(t);
            _frequency.push_back(1);
        } else {
            size_t idx = to_index(t);
            _frequency[idx]++;
        }
    }

    template <typename OutputIt>
    void weights(OutputIt d_first) const {
        make_weights(_data.begin(), _data.end(), d_first);
    }
};
