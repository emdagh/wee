#pragma once

namespace wee {
    template <typename R, typename... Args>
    std::function<R (Args...)> memoize(R(*fn)(Args...)) {
        std::unordered_map<std::tuple<Args...>, R> storage;
        return [fn, storage] (Args... args) -> R 
        {
            auto arg = std::make_tuple(args...);
            auto it = storage.find(arg);
            if(it == storage.end()) {
                // auto res = fn(args...);
                // storage[arg] = res;
                // return res;
                storage.insert(std::make_pair(
                    arg, fn(args...)
                ).first->second);
            } else {
                return it->second;
            }
        }
    }
}
