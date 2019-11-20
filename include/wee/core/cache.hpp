#pragma once

namespace wee {

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
    cache(size_t capacity, erase_cb cb) 
        : _capacity(capacity) 
        , on_erase(cb)
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

    size_t address_of(const key_type& k) {
        return _policy.where(k);
    }
};

}
