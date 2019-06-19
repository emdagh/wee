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
}
