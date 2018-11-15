#pragma once

#include <core/singleton.hpp>
#include <string>
#include <map>
#include <functional>

namespace wee {


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

}
