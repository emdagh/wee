#pragma once

#include <tuple>
#include <array>

namespace wee {

    template <std::size_t... Ns, typename... Ts>
    constexpr auto head_impl(std::index_sequence<Ns...>, const std::tuple<Ts...>& t) {
        return std::make_tuple(std::get<Ns>(t)...);
    }

    template < typename T , typename... Ts >
    constexpr auto head( const std::tuple<T,Ts...>& t ) {
        //return  std::get<0>(t);
        return head_impl(std::make_index_sequence<sizeof...(Ts)>(), t);
    }

    template < std::size_t... Ns , typename... Ts >
    constexpr auto tail_impl( std::index_sequence<Ns...> , const std::tuple<Ts...>& t ){
        return  std::make_tuple( std::get<Ns+1u>(t)... );
    }

    template < typename... Ts >
    constexpr auto tail( const std::tuple<Ts...>& t ) {
        return  tail_impl( std::make_index_sequence<sizeof...(Ts) - 1u>() , t );
    }

    template <typename T, size_t... Is>
    constexpr auto make_tuple_impl(const T& t, std::index_sequence<Is...>) {
        return std::make_tuple(t[Is]...);
    }

    template <typename T, size_t N>
    constexpr auto make_tuple(const std::array<T, N>& a) {
        return make_tuple_impl(a, std::make_index_sequence<N>{});
    }
    


    template <typename T>
    constexpr auto make_array(T&& t) {
        constexpr auto expand = [](auto&&... x) {
            return std::array { std::forward<decltype(x)>(x) ... };
        };
        return std::apply(expand, std::forward<T>(t));
    }

    template<typename T, size_t... I>
    auto constexpr tuple_reverse_impl(T&& t, std::index_sequence<I...>) {
        //using type = typename std::decay<T>::type;
        return std::make_tuple(std::get<sizeof...(I) - 1 - I>(std::forward<T>(t))...);
    }
    template<typename T>
    auto constexpr tuple_reverse(T&& t) {
        using type = typename std::decay<T>::type;

        if constexpr(std::tuple_size<type>::value != 0)
          return tuple_reverse_impl(std::forward<T>(t), std::make_index_sequence<std::tuple_size<type>::value>());
    }

    /**
     * superceded by C++17 std::apply
     */
    template <typename T, typename F, size_t... Is>
    [[deprecated("use std::apply")]] 
    void for_each_impl(F&& f, T&& t, std::index_sequence<Is...>) {
        static_cast<void>(std::initializer_list<int> {
            (f(std::get<Is>(std::forward<T>(t))), void(), int {})...
        });
    }

    template <typename F, typename T>
    [[deprecated("use std::apply")]] 
    void for_each(F&& f, T&& t) {
        constexpr size_t N = std::tuple_size<std::decay<T> >::value;
        for_each_impl(std::forward<F>(f), std::forward<T>(t), std::make_index_sequence<N>());
    }

}
