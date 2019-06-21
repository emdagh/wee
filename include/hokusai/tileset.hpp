#pragma once

#include <set>

template <typename T>
struct tileset {
    /**
     * TODO: can we replace all this with a single std::unordered_multiset?
     */
private:
    std::vector<T> _data;
    std::unordered_map<T, size_t> _names;
    //std::map<T, float> _frequency;
    std::vector<float> _frequency;
public:

    tileset() { 
        push(0); 
    }

    const std::vector<T> data() const { return _data; }

    T to_tile(size_t i) const { return _data[i]; }
    size_t to_index(T t) const { return _names.at(t); }//_data.at(_names.at(t)); }

    size_t frequency_of(T t) {
        return _frequency[this->to_index(t)];
    }

    void set_frequency(T t, float f) { 

        _frequency[this->to_index(t)] = f;
    }
    /**
     * returns the frequencies for each _index_
     */
    const std::vector<float> frequencies() const { 
#if 1 
        return _frequency;
#else
        std::vector<float> res(_frequency.size());
        for(auto& kv : _frequency) {
            res[this->to_index(kv.first)] = kv.second;
        }
        return res;
#endif
    }
    size_t length() const { return _data.size(); }
    
    /*template <typename InputIt, typename OutputIt>
    void make_weights(InputIt first, InputIt last, OutputIt d_first) const {
        std::multiset<typename InputIt::value_type> temp(first, last);
        for(auto it = temp.begin(); it != temp.end(); it = temp.upper_bound(*it)) {
            auto ti = _names[*it];
            *(d_first + ti) = temp.count(*it);
        }
    }*/
    
    void push(T t) {
        if(_names.count(t) == 0) {
            _names.insert(std::make_pair(t, _data.size()));
            _data.push_back(t);
            _frequency.push_back(1);
        } else {
            _frequency[this->to_index(t)]++;
        }
    }


    template <typename Iter>
    static tileset make_tileset(Iter first, Iter last) {
        Iter ptr = first;
        tileset res;
        while(ptr != last) {
            res.push(*ptr++);
        }
        return res;
    }
};
