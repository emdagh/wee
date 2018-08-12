#pragma once

#include <core/singleton.hpp>
#include <string>
#include <map>

namespace atom {


template <typename T, typename ClassId = std::string>
struct factory 
: public singleton<factory<T, ClassId> >
{
    typedef T*(*create_fn)();
    typedef ClassId classid_t;
    
    void register_class(const classid_t& id, create_fn fn) {
        auto it = _reg.find(id);
        
        if(it != _reg.end()) 
            return;
        _reg[id] = fn;
    }

    T* create(const classid_t& id) {
        auto it = _reg.find(id);
        if(it != _reg.end() ) 
            return (*it).second();
        return NULL;
    }
    
protected:
    std::map<ClassId, create_fn> _reg;
};

template <typename S, typename T, typename ClassId = std::string>
struct register_factory {
    typedef S base_t;
    typedef T derived_t;
    typedef ClassId classid_t;

    explicit register_factory(const classid_t& id) {
        factory<base_t, classid_t>::instance().register_class(id, register_factory<S, T, ClassId>::create);
    }

    register_factory(const classid_t& id, typename factory<base_t, classid_t>::create_fn fn) {
        factory<base_t, classid_t>::instance().register_class(id, fn);        
    }

protected:
    static base_t* create() {
        return new derived_t;
    }
};



}
