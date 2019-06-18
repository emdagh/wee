#pragma once

#include <wee/wee.h>
#include <exception>
#include <stdexcept>   
// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
#define WEE_HELPER_DLL_IMPORT __declspec(dllimport)
#define WEE_HELPER_DLL_EXPORT __declspec(dllexport)
#define WEE_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define WEE_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define WEE_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define WEE_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define WEE_HELPER_DLL_IMPORT
#define WEE_HELPER_DLL_EXPORT
#define WEE_HELPER_DLL_LOCAL
#endif
#endif

#ifdef WEE_LIB_SHARED // defined if wee is compiled as a shared object
#ifdef WEE_LIB_EXPORTS // defined if we are building the wee shared object (instead of using it)
#define WEE_API WEE_HELPER_DLL_EXPORT
#else
#define WEE_API WEE_HELPER_DLL_IMPORT
#endif
#define WEE_LOCAL WEE_HELPER_DLL_LOCAL
#else 
#define WEE_API
#define WEE_LOCAL
#endif // FOX_DLL

#ifdef _MSC_VER
#pragma warning (disable: 4146)
#   define ALWAYS_INLINE __forceinline
#elif defined(__GNUC__)
# define ALWAYS_INLINE __attribute__((always_inline)) inline
#else
#   define ALWAYS_INLINE inline
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

    struct WEE_API not_implemented : std::logic_error {
        not_implemented() : std::logic_error("function or method not implemented") {

        }
    };

    struct WEE_API file_not_found : std::runtime_error {
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
