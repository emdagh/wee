#pragma once

namespace wee
{
  namespace detail
{
    template <typename R, typename T, typename... Args>
    struct lambda_traits
    {
        enum { arity = sizeof...(Args) };
        using return_type = R;
        using args = std::tuple<Args...>;
    };
}

template <typename T>
struct closure_traits : closure_traits<decltype(&T::operator())> {};

template <typename R, typename T, typename... Args>
struct closure_traits<R(T::*)(Args...)> : detail::lambda_traits<R, T, Args...>{};

template <typename R, typename T, typename... Args>
struct closure_traits<R(T::*)(Args...) const> : detail::lambda_traits<R, T, Args...>{};

}
