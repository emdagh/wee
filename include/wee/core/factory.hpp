#pragma once

#include <core/singleton.hpp>
#include <string>
#include <map>
#include <functional>
#include <wee.hpp>

namespace wee {
    template <typename T, typename S=std::string, typename... Args>
    struct factory {
        typedef std::function<T*(Args...)> fn;
        std::unordered_map<S, fn> _create;

        factory() = default;

        DISALLOW_COPY_AND_ASSIGN(factory);

        static factory& instance() {
            static factory* ptr = new factory;
            return *ptr;
        }

        void register_class(const S& key, const fn& value) {
            if(auto it=_create.count(key); it > 0)
                return;
            _create.insert(std::make_pair(key, value));//at(key) = value;
        }

        T* create(const S& key, const Args&... args) {
            return _create.count(key) ? _create.at(key)(args...) : nullptr;
        }
    };
    // sugar
    template <typename T, typename Q, typename S>
    void register_factory(const S& key) {
        factory<T, S>::instance().register_class(key, [] () { return dynamic_cast<T*>(new Q); });
    }
/*

template <typename T, typename ClassId = std::string, typename... Args>
struct factory 
: public singleton<factory<T, ClassId, Args...> >
{
    typedef std::function<T*(Args...)> create_fn; //T*(*create_fn)(void);
    typedef ClassId classid_t;
    
    void register_class(const classid_t& id, const create_fn& fn) {
        auto it = _reg.find(id);
        
        if(it != _reg.end()) 
            return;
        _reg[id] = fn;
    }

    T* create(const classid_t& id, const Args&... args) {
        auto it = _reg.find(id);
        if(it != _reg.end() ) 
            return (*it).second((args)...);
        return NULL;
    }
    
protected:
    std::map<ClassId, create_fn> _reg;
};


template <typename S, typename T, typename C = std::string>
void register_factory(const C& id) {
    factory<S, C>::instance().register_class(id, [] (void) { return new T; });
}
*/
}
