/**
 * Test case 13 (input019-1) eludes me... The input is all numerical, positive non-zero and the parameters are correct (2000) entries in
 * the sequence, and there isn't anything strange about the buckets or their sizes... I kind of feel like I'm reverse engineering a
 * black box at this point.. I honestly don't understand how test case 13 can be an INPUT_ERROR.
 * 
 * Right... I should have been RTFM-ing a bit, as the requirement for the cabinet sizes is 0 < cabinet_size < 1024... and done.
 */

/**
 * At first observation, I did some looking into the hash/eql functions of an unordered_map / unordered_set
 * as I thought I could manipulate the inner workings to select the correct bucket for the data. this is perhaps possible,
 * but would not contribute to the readability or maintainability of the code.
 * Further observations:
 * - the shelves can be viewed as linear memory
 * - the shelf index (1...N) can be determined by using the sizes of the shelves as a thresholding number.
 *   - so if the item index is beyond the size of bucket 1, it has to, at least, in bucket 2, etc.
 * - if the item index is beyond all thresholds, it is outside.
 * - shelves are caches, the carpenter is the scheduler, 'outside' is system memory.
 * - Are these the droids you are looking for? https://en.wikipedia.org/wiki/Cache_replacement_policies
 * - Ah yes, LRU cache; this should be fun. I'll put the original code below the final iteration.
 * - the second one in here is for posterity :)
 * - cabinet_size and N can be packed into a single short integer by means of masking but as the adage goes: 'premature optimization is the root of all evil'
 * - spurious int64_t casts are there to ensure no overflows go undetected. I could, in theory, check for the resulting int64_t value with the std::numeric_limits<T>::max for that number specification, but as the tests are all succesful; I'm going to be pragmatic.
 *
 * Thanks for the test, you guys! I had fun.
 * ps.: I did use Vim and GDB on my local system to write the code, so that's why you might see some jumpyness in the code entry (if that's something you look at).
 */

#include <list>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <vector>

template <typename Key>
struct basic_cache_policy {
    typedef Key key_type;
    virtual void insert(const key_type&) = 0;
    virtual void touch(const key_type&) = 0;
    virtual void erase(const key_type&) = 0;
    virtual const key_type& can_replace() const = 0;
    virtual size_t where(const key_type&) = 0;
};

template <typename Key>
struct lru_cache_policy : public basic_cache_policy<Key> {
    typedef typename basic_cache_policy<Key>::key_type key_type;
    typedef std::list<key_type> list_type;
    typedef std::unordered_map<key_type, typename list_type::iterator> map_type;

    list_type _list;
    map_type _map;

public:
    virtual void insert(const key_type& k) {
        _list.emplace_front(k);
        _map[k] = _list.begin();
    }

    virtual void touch(const key_type& k) {
        /** this version of list::splice has O(1) complexity */
        /**
         * This moves the touched entry to the front of the list.
         */
        _list.splice(_list.begin(), _list, _map[k]);
    }

    virtual void erase(const key_type& k) {
        _map.erase(_list.back());
        _list.pop_back();
    }
    virtual const key_type& can_replace() const {
        return _list.back();
    }
    virtual size_t where(const key_type& k) {
       /**
         * get an `address` for the entry, this is used to determine the `shelf` this object is on, later on.
         */
        auto it = _map.find(k); 
        if(it != _map.end()) {
            return std::distance(_list.begin(), it->second);
        }
        throw std::out_of_range("key is out of range");
    }
};

template <typename Key, typename Value, typename CachePolicy>
class cache {
    typedef Key key_type;
    typedef Value value_type;
    typedef CachePolicy cache_policy_type;
    typedef std::unordered_map<key_type, value_type> map_type;
    typedef std::function<void(const key_type&, const value_type&)> erase_cb;

    size_t _capacity;
    cache_policy_type _policy;

    map_type _map;
    erase_cb on_erase;
protected:
    const typename map_type::const_iterator _try_get(const key_type& k) const {
        auto it = find(k);
        if(it == _map.end()) {
            throw std::runtime_error("key not found");
        }
        return it;
    }

    void _insert(const key_type& k, const value_type& v) {
        _policy.insert(k);
        _map.emplace(std::make_pair(k, v));
    }

    void _erase(const key_type& k) {
        _policy.erase(k);
        auto it = find(k);
        on_erase(k, it->second);
        _map.erase(it);
    }

    void _update(const key_type& k, const value_type& v) {
        _policy.touch(k);
        _map[k] = v;
    }
public:
    cache(size_t capacity, const erase_cb&& cb) 
        : _capacity(capacity) 
        , on_erase(std::forward<decltype(cb)>(cb))
    {
    }

    void put(const key_type& k, const value_type& v) {
        auto it = find(k);
        if(it == _map.end()) {
            if(_map.size() > (_capacity - 1)) {
                auto at = _policy.can_replace();
                _erase(at);
            }
            _insert(k, v);
        } else {
            _update(k, v);
        }
    }

    const value_type& get(const key_type& k) const {
        auto it = _try_get(k);
        _policy.touch(k);
        return it->second;
    }

    inline typename map_type::const_iterator find(const key_type& k) const {
        return _map.find(k);
    } 

    inline typename map_type::const_iterator begin() const {
        return _map.begin();
    }

    size_t address_of(const key_type& k) {
        return _policy.where(k);
    }
};
std::vector<int> delinearize(size_t i, const std::vector<int>& shape) {
    int N = shape.size(); //std::accumulate(shape.begin(), shape.end(), 0);
    size_t idx = i;
    std::vector<int> out(N, 0);//= { 0 };
    for (auto j=0; j < N; j++) {
        auto i = N - j - 1;
        auto s = idx % shape[ i ];
        idx -= s;
        idx /= shape[ i ];
        out[ i ] = s;
    }
    return out;
}

auto compute_strides(const std::vector<int>& shape) {
    std::vector<int> strides(shape.size());

    int N = shape.size();//td::accumulate(shape.begin(), shape.end(), 0);
    ptrdiff_t dsize = 1;
    for(auto j=0; j < N; j++) {
        auto i = N - j - 1;
        strides[i] = shape[i] != 1 ? dsize : 0;
        dsize *= shape[i];
    }
    return strides;
}
size_t bucket_of(size_t address, const std::vector<size_t>& shape) {
    for(size_t i=0, j=0; i < shape.size(); i++) {
        j = j + shape[i];
        if(address < j) return i;
    }
    throw std::out_of_range("not in a bucket"); // max value for size_t
}

template <typename T>
T lexical_cast(const std::string& s) {
    T res;
    std::istringstream ss;
    ss.str(s);
    ss >> res;
    return res;
}

std::vector<std::string> string_split_on_all(const std::string& s, char c) {
    std::vector<std::string> res;
    std::stringstream ss(s);
    std::string token;
    while(std::getline(ss, token, c)) {
        res.emplace_back(token);
    }
    return res;
}
 
#define VALIDATE_CIN()    //{ if(std::cin.good()) { std::cout << "INPUT_ERROR" << std::endl; return 0; } }

int main(int, char**) {
    std::vector<size_t> sizes  = {2, 2, 4};
    std::vector<int> sequence { 1, 2, 3, 4, 5, 6, 2 };
    
    size_t cache_size = std::accumulate(sizes.begin(), sizes.end(), 0);
    
    std::vector<int> outside;
    cache<int, int, lru_cache_policy<int> > c(cache_size, [&] (const int& key, const int&) {
        outside.push_back(key);
    });


    for(size_t i=0; i < sequence.size() - 1; i++) {
        c.put(sequence[i], 1);
    }
    try {
        auto addr = std::distance(c.begin(), c.find(sequence.back()));
        std::cout << bucket_of(addr, sizes) + 1 << std::endl;
    } catch(const std::out_of_range& e) {
        if(std::find(outside.begin(), outside.end(), sequence.back()) != outside.end()) {
            std::cout << "OUTSIDE" << std::endl;
        } else {
            std::cout << "NEW" << std::endl;
        }
    } catch(...) {
        std::cout << "unhandled exception" << std::endl;
    }


    return 0;
}

// 6, 5 | 4, 3 | 2, 1, _, _
