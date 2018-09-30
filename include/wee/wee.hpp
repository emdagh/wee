#pragma once

#include <wee/wee.h>
#include <exception>
#include <stdexcept>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&) = delete;        \
void operator=(TypeName) = delete

namespace { // prevent cluttering of global namespace

    struct not_implemented : std::logic_error {
        not_implemented() : std::logic_error("function or method not implemented") {
        }
    };

    struct file_not_found : std::runtime_error {
        file_not_found(const std::string& what) : std::runtime_error("file not found: " + what) {}
    };


    template <typename T>
    T& as_lvalue(T&& t) {
        return t;
    }
    template <typename T>
    T zero() {
        return std::pair<T,T>().first;
    }

    template <typename T>
    T one() {
        return !zero<T>();
    }

}
