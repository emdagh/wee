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

    T* create(const classid_t& id, const Args&&... args) {
        auto it = _reg.find(id);
        if(it != _reg.end() ) 
            return (*it).second(std::forward<Args>(args)...);
        return NULL;
    }
    
protected:
    std::map<ClassId, create_fn> _reg;
};

//struct <typename S, typename T>
///static S* factory_basic_create() { return new T(); }

/*template <typename S, typename T, typename C = std::string, typename Args...>
void register_factory(const C& id) {
    factory<S, C>::instance().register_class(id, [] (const Args&&... args) { 
        return dynamic_cast<S*>(new T(args...)); 
    });
}*/

template <typename S, typename T, typename C = std::string>
void register_factory(const C& id, const typename factory<S, C>::create_fn& fn) {
    factory<S, C>::instance().register_class(id, fn);
}


/*template <typename S, typename T, typename ClassId = std::string>
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
};*/



}
