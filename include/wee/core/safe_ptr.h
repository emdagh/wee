#include <memory>
#include <mutex>
#include <vector>
#include <thread>
#include <iterator>
#include <iostream>

namespace wee {

template <typename T, typename L>
class safe_proxy {
    T* _m;
    std::unique_lock<L> _lock;
public:
    safe_proxy(T* p, L& mtx) : _m(p), _lock(mtx) {}
    T* operator -> () { return _m; }
};

template <typename T> 
class safe_ptr {
    std::unique_ptr<T> _m;
    std::recursive_mutex _lock;
public:
    template <typename... Args>
    safe_ptr(Args... args) : _m(std::make_unique<T>(args...)) {}

    safe_proxy<T, decltype(_lock)> operator -> () { 
        return safe_proxy(_m.get(), _lock); 
    }

    T& get() { return *(_m.get()); }
};
} // wee
/*
template <typename T>
using safe_vector = safe_ptr<std::vector<T> >;

int main(int, char**) {
    safe_vector<int> vec;

    srand(1024);

    std::thread t0([&] {
        for(int i=0; i < 32; i++) {
            vec->push_back(37);
            auto ms = std::chrono::milliseconds(rand() & 127);
            std::this_thread::sleep_for(ms);
        }
    });

    std::thread t1([&] {
        for(int i=0; i < 32; i++) {
            vec->push_back(13);
            auto ms = std::chrono::milliseconds(rand() & 127);
            std::this_thread::sleep_for(ms);
        }
    });

    t0.join();
    t1.join();

    std::copy(vec->begin(), vec->end(), std::ostream_iterator<int>(std::cout, ","));

    std::cout << std::endl << "len=" << vec->size() << std::endl;

    return 0;
    
}*/
