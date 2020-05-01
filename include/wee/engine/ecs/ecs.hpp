#ifndef __WEE_ECS_INCLUDED
#define __WEE_ECS_INCLUDED

#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <vector>

namespace wee::ecs {
    struct entity;
}

namespace std {
    template <>
    struct hash<wee::ecs::entity> {
        std::size_t operator () (const wee::ecs::entity& e) const;
    };

    template <>
    struct equal_to<wee::ecs::entity> {
        bool operator () (const wee::ecs::entity&, const wee::ecs::entity&) const;
    };
    
}


namespace wee::ecs {
    using id_type = intptr_t;

    template <typename T>
    using set = std::unordered_set<T>;

    template <typename K, typename V>
    using map = std::unordered_map<K, V>;

    template <typename T>
    constexpr T zero() {
        return T {};
    }

    template <typename T = id_type>
    constexpr T none() {
        return zero<T>();
    }

    inline id_type id() {
        static id_type i = zero<id_type>();
        return ++i;
    }

    struct entity {
        id_type _id;
        
        static std::unordered_set<entity*>& all() {
            static std::unordered_set<entity*> t;
            return t;
        }
        
        entity(const id_type& id_ = id()) : _id(id_) {
            all().insert(this);
        }
        
        ~entity() {
            all().erase(this);
        }

        bool operator == (const entity& other) const { return _id == other._id; }
        
        operator id_type () const { return _id; }
    };



    template <typename T>
    set<entity>& any() {
        static set<entity> s;
        return s;
    }

    enum {
        UNION,
        INTERSECT,
        DIFFERENCE
    };


    template <typename T>
    T set_difference(const T& a, const T& b) {
        using key_type = typename T::value_type;
        if(a.size() <= b.size()) {
            T res;
            std::copy_if(a.begin(), a.end(), std::inserter(res, res.end()), [&b] (const key_type& k) {
                return b.count(k) == 0;
            });
            return res;
        }
        return set_intersect(b, a);
    }


    template <typename T>
    T set_intersect(const T& a, const T& b) {
        using key_type = typename T::value_type;
        if(a.size() <= b.size()) {
            T res;
            std::copy_if(a.begin(), a.end(), std::inserter(res, res.end()), [&b] (const key_type& k) {
                return b.count(k) != 0;
            });
            return res;
        }
        return set_intersect(b, a);
    }

    template <typename T>
    T set_union(const T& a, const T& b) {
        T res { a };
        res.insert(b.begin(), b.end());
        return res;
    }


    template <int T>
    set<entity> group_by(const set<entity>& a, const set<entity>& b) {
        if constexpr(T == UNION) { return set_union(a, b); }
        else if(T == INTERSECT)  { return set_intersect(a, b); }
        else if(T == DIFFERENCE) { return set_difference(a, b); }
        else { throw std::runtime_error("unsupported set operation"); }
        return {};
    }

    template <typename T, typename... Ts>
    set<entity> join() { 
        if constexpr(sizeof...(Ts) == 0) {
            return any<T>(); 
        } else {
            return group_by<INTERSECT>(any<T>(), join<Ts...>());
        }
    }

    template <typename T, typename... Ts>
    set<entity> join(T first, Ts... rest) {
        return join<T, Ts...>();
    }

    template <typename T>
    map<id_type, T>& components() {
        static map<id_type, T> m;
        return m;
    }

    template <typename T>
    typename T::value_type& get(const id_type& id) {
        return components<T>()[id]._val;
    }

    template <typename T>
    typename T::value_type& add(const id_type& id) {
        any<T>().insert(id);
        return get<T>(id);
    }

    template <typename T>
    bool has(const id_type& id) {
        return components<T>().count(id) > 0;
    }

    struct interface {
        virtual ~interface() {}
        static std::vector<interface*>& registered() {
            static std::vector<interface*> lst;
            return lst;
        }
    };

    template <size_t S, typename T>
    struct component : interface {
        typedef T value_type;
        value_type _val;
        
        component(bool reentrant = 0) {
            if(!reentrant) {
                static struct registerme {
                    registerme() {
                        interface::registered().push_back(new component(1));
                    }
                } _st;
            }
        }
        
        ~component() {
            auto& lst = interface::registered();
            for(auto& it : lst) {
                if(this == it) {
                    std::swap(it, lst.back());
                    lst.pop_back();
                }
            }
        }
    };
}

#endif