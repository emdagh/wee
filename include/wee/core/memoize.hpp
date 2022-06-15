#pragma once

#include "closure_traits.hpp"

namespace wee {
template <typename F>
auto memoize(F fun)
{
    using key_t = typename closure_traits<F>::args;
    using val_t = typename closure_traits<F>::return_type;

    static std::unordered_map<key_t, val_t, hash<key_t>> cache;
    
    return [fun = std::forward<F>(fun)] (auto... args) 
    {
        auto key = std::tuple(args...);
        if(auto it = cache.find(key); it != cache.end())
        {
            std::cout << "returning cached value" << std::endl;
            
            return it->second;
        }
        std::cout << "first occurence" << std::endl;
        return cache.insert(std::make_pair(key, std::apply(fun, key))).first->second;
    };
}
}
