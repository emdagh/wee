#pragma once

template <typename T, typename After>
class call_proxy {
    T ptr;
    After after;
public:
    call_proxy(T ptr, After after)
    : ptr(ptr)
    , after(after) {}

    call_proxy(const call_proxy&) = delete;
    call_proxy& operator = (const call_proxy&) = delete;

    ~call_proxy() { after(); }

    T operator -> () { return ptr; }
    
};

template <typename T, typename Before, typename After>
class wrap_ptr {
    T ptr;
    Before before;
    After after;
public:
    wrap_ptr(T ptr, Before before, After after) 
    : ptr(ptr)
    , before(before)
    , after(after) {}

    call_proxy<T&, After&> operator -> () {
        before();
        return call_proxy<T&, After&>(ptr, after);
    }
};

template <typename T, typename... Args>
wrap_ptr make_wrap(Args&&... args) {
    return wrap_ptr(new T(std::forward<Args>(args)...));
}

/*
#include <mutex>
#include <vector>
#include <iostream>

int main() {
    std::mutex m;
    std::vector<int> vec;
    wrap_ptr ea(&vec, 
    [&] { 
        std::cout << "lock" << std::endl; 
        m.lock(); 
    }, 
    [&] {
        std::cout << "unlock" << std::endl; 
        m.unlock();
    });
    ea->push_back(10);
    return 0;
}
*/
