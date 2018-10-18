#pragma once

#include <future>

namespace cpp {

    template <typename T, typename F>
    void scoped_lock(T& lock, F fun) {
        std::unique_lock<T> l(lock);
        fun();
    }
    
    template <typename I, typename F>
    void parallel_foreach(I first, I last, F fun) {
        std::vector<std::future<void> > fut;
        fut.reserve(std::distance(first, last));
        for(; first != last; ++first) {
            fut.push_back(std::move(std::async(std::launch::async, fun, *first)));
        }
    }
    
    template <typename I, typename S, typename F>
    void parallel_foreach_n(I first, S n, F fun) {
        std::vector<std::future<void> > fut;
        fut.reserve(n);

        for(S i=0; i < n; ++first, ++i) {
            fut.push_back(std::move(std::async(std::launch::async, fun, *first)));
        }
    }
}
