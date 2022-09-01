#pragma once
#include <functional>
#include <type_traits>
#include <utility>

//ref: http://tclamb.github.io/blog/2013/09/02/lambda-type-erasure/

namespace wee
{
    /* SFNIAE helper struct for call signature extraction of
     * member functions */
    template<typename T> struct remove_class {};

    template<typename R, typename C, typename... A>
    struct remove_class<R(C::*)(A...)>
    {
        using type = R(A...);
    };

    template<typename R, typename C, typename... A>
    struct remove_class<R(C::*)(A...) const>
    {
        using type = R(A...);
    };

    /* lambda functions are never volatile (see 5.1.2:5 of the standard)
     * these specializations are provided for completeness */
    template<typename R, typename C, typename... A>
    struct remove_class<R(C::*)(A...) volatile>
    {
        using type = R(A...);
    };

    template<typename R, typename C, typename... A>
    struct remove_class<R(C::*)(A...) const volatile>
    {
        using type = R(A...);
    };
}

template<typename F>
using function_t = std::function< typename remove_class<
    decltype( &std::remove_reference<F>::type::operator() )
>::type >;

template <typename F>
function_t<F> make_function(F&& functor) {
    return {std::forward<F>(functor)};
}
