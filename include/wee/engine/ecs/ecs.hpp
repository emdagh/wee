#pragma once


#include <unordered_set>
#include <unordered_map>
#include <core/set.hpp>
#include <iostream>

namespace wee::ecs {

    using id_type = uintptr_t;

    template <typename T = id_type>
    T zero() {
        return T {};
    }

    template <typename T = id_type>
    T none() {
        return zero<T>();
    }

    template <typename T = id_type>
    T& create_entity() {
        static T id = none<T>();
        return ++id;
    }

    struct entity {
        id_type _id;
        static auto& all() {
            static std::unordered_set<entity*> statics;
            return statics;
        }
        entity(id_type id_ = create_entity()) : _id(id_) { all().insert(this); }
        ~entity() { all().erase(this); }
        operator id_type() const { return _id; }

        bool operator == (const entity& e) const { return _id == e._id;}
    };

    struct entity_hash { 
        size_t operator () (const entity& e) const { 
            return std::hash<decltype(e._id)>()(e._id); 
        } 
    };
    struct entity_equal_to { 
        bool operator () (const entity& a, const entity& b) const { 
            return a._id == b._id; 
        } 
    };

    template <typename T>
    auto& any() {
        static std::unordered_set<entity, entity_hash, entity_equal_to> entities;
        return entities;
    }

    template <typename T>
    auto& components() {
        static std::unordered_map<id_type, T> res;
        return res;
    }

    template <typename T>
    typename T::value_type& get(const id_type& id) {
        return components<T>()[id]._value;
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

    template <size_t S, typename T>
    struct component {
        using value_type = T;
        value_type _value;
    };

    template <typename T, typename... Ts>
    auto join() {
        if constexpr(sizeof...(Ts) == 0) {
            return any<T>();
        } else {
            return set_intersect(any<T>(), join<Ts...>());
        }
    }
}