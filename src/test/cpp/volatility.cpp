#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <cassert>
/**
 * solution based on https://www.johndcook.com/blog/standard_deviation/
 * Welford's online algorithm. Cool stuff, thanks for that.
 */
template <typename T>
class volatility_tracker {
    size_t _period;
    size_t _time;
    T _mean;
    T _var;

    std::deque<T> _q;
protected:
    void _add(T x) {
        _q.push_front(x);

        T next = _mean + (x - _mean) / _q.size();
        _var = _var + (x - _mean) * (x - next);
        _mean = next;
    }

    void _remove(T x) {
        if(_q.size() == 1) {
            _mean = _var = 0.0f;
        } else {
            T prev = (_q.size() * _mean - x) / (_q.size() - 1);
            //T delta = (x - _mean) * (x - prev);
            _var = _var - (x - _mean) * (x - prev);
            _mean = prev;
        }
    }
public:
    /**
     * @param - max period to keep data for (4320 for example)
     */
    explicit volatility_tracker(size_t max_period) 
        : _period(max_period)
        , _time(0)
        , _mean(0.0f)
        , _var(0.0f)
        
    {
    }
    void add_point(T price, size_t timestamp) {
        assert(timestamp > _time);
        _time = timestamp;
        if(_q.size() >= _period) {
            _remove(_q.back());
            _q.pop_back();
        }
        _add(price);
    }

    T get_volatility() {
        /**
         * This https://www.macroption.com/is-volatility-variance/
         * threw me off a bit, but using the stddev is probably a Bad Idea, 
         * because it introduces a sqrt to the algorithm. 
         * Comparing squared values to var() is a lot faster (and more stable I suppose...)
         */
#define _VOLATILITY_AS_VARIANCE
#ifdef _VOLATILITY_AS_VARIANCE
        return var();
#else
        return sd(); 
#endif
    }


    T var() const {
        return (_q.size() > 1) ? _var / (_q.size() - 1) : 0.0f;
    }

    T sd() const {
        return std::sqrt(var());
    }
};

/**
 * algorithms to validate output data.
 */
template <typename InputIt>
float mean(InputIt first, InputIt last) {
    InputIt it = first;
    float sum = 0.f;
    while(it != last) {
        sum += *it++;
    }
    return static_cast<float>(sum) / std::distance(first, last);
}

template <typename InputIt>
float sample_var(InputIt first, InputIt last) {
    auto mn = mean(first, last);
    InputIt it = first;
    float tmp = 0;
    while(it != last) {
        tmp += (*it - mn) * (*it - mn);
        it++;
    }
    return tmp / (std::distance(first, last) - 1);
}
/**
 * driver
 */
int main(int argc, char** argv) {
    /**
     */
    auto v = volatility_tracker<float>(5);
    size_t i=0;
    std::vector<float> input = { 55, 39, 42, 35, 58, 55, 39, 42}; 
    for(auto value : input) {
        v.add_point(value, ++i);
    }
    std::cout << v.get_volatility() << std::endl;

    assert(v.get_volatility() == 102.7f);
    
    return 0;
}
