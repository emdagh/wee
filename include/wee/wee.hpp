#pragma once

#include <wee/wee.h>
#include <exception>
#include <stdexcept>

#ifdef __GNUC__
#define __clz(x)        __builtin_clz(x)
#define __ctz(x)        __builtin_ctz(x)
#define __popcount(x)   __builtin_popcount(x)
#else
static uint32_t ALWAYS_INLINE popcnt( uint32_t x )
{
    x -= ((x >> 1) & 0x55555555);
    x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
    x = (((x >> 4) + x) & 0x0f0f0f0f);
    x += (x >> 8);
    x += (x >> 16);
    return x & 0x0000003f;
}
static uint32_t ALWAYS_INLINE clz( uint32_t x )
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return 32 - popcnt(x);
}
static uint32_t ALWAYS_INLINE ctz( uint32_t x )
{
    return popcnt((x & -x) - 1);
}
#define __popcount(x) popcnt(x)
#endif

#define WEE_DEFAULT_COPY_AND_ASSIGN(T) \
    T(const T&) = default; \
    T& operator = (const T&) noexcept = default; \
    T(T&&) = default; \
    T& operator = (T&&) noexcept = default;
    

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
