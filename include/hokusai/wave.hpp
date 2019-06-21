#pragma once


template <typename T>
struct wave {
    std::vector<T> _data;
    wee::random _rand;//  = { 600089286 };

    wave(size_t n, T t) : _data(n, t)
    {
        DEBUG_VALUE_OF(_rand.seed());
    }

    const auto& data() const { return _data; }

    auto avail_at(size_t i) const { 
        std::vector<T> res;
        avail(_data[i], std::back_inserter(res)); 
        return res;
    }

    void pop(size_t i, T t) { wee::pop_bits(_data[i], t); }

    void reset(T t) { std::fill(_data.begin(), _data.end(), t); }
    
    size_t length() const { return _data.size(); }

    bool is_collapsed_at(size_t i) const {
        return is_collapsed(_data[i]);
    }

    size_t collapse(const std::vector<float>& weights) {
        size_t i = min_entropy_index();

        //std::unordered_map<int, float> w;
        std::map<int, float> w;
        float total_weight = 0.f;
        auto options = avail_at(i);//_wave->_data[i]);
        for(auto t: options) {
            w.insert(std::pair(t, weights[t]));
            total_weight += weights[t];
        }
        float random = _rand.next<float>(0.f, 1.0f) * total_weight;

        for(const auto& [key, val]: w) {
            random -= val;
            if(random < 0) {
                collapse_at(i, to_bitmask<T>(key));
                return i;
            }
        }
        DEBUG_VALUE_OF(_data);
        throw std::runtime_error("wave could not collapse any further...");
    }

    void collapse_at(size_t i, T t) { _data.at(i) = t; }

    bool is_collapsed(T t) const { return popcount(t) == 1; }

    size_t min_entropy_index() {
        size_t ret = 0;
        float min_h = std::numeric_limits<float>::infinity();
        for (auto i : range(_data.size())) {
            if (is_collapsed(_data[i])) 
                continue;

            float h = entropy_of(_data[i]) - _rand.next<float>(0.0f, 1.0f) / 1000.0f;
            if (h < min_h) {
                min_h = h;
                ret = i;
            }
        }
        return ret;
    }

    T any_possible(size_t i, T t) const { return _data[i] & t; }
    bool is_same(size_t i, T t) const { return _data[i] == t; }
};
