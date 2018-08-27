#pragma once

#include <wee.hpp>

namespace wee {

template <typename T>
struct singleton {
    static T& instance() {
        return *instance_ptr();
    }

    static T* instance_ptr() {
        static T* _instance = new T;
        return _instance;
    }
protected:
    singleton()
    {
    }
    DISALLOW_COPY_AND_ASSIGN(singleton<T>);
    virtual ~singleton() = default;
};


}
