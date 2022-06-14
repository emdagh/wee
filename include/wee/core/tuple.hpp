#pragma once

#include <tuple>
#include <array>

namespace wee {
    
    template <class T>
constexpr inline std::size_t hash_combine(T const& v,
  std::size_t const seed = {}) noexcept
{
  return seed ^ (std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

template <typename T> struct hash;

template <typename ...T>
struct hash<std::tuple<T...>>
{
  template <typename A1, typename ...A, std::size_t ...I>
  static auto apply_tuple(std::tuple<A1, A...> const& t,
    std::index_sequence<I...>) noexcept
  {
    if constexpr(sizeof...(A))
    {
      return hash_combine(std::get<0>(t),
        hash<std::tuple<A const&...>>()({std::get<I + 1>(t)...})
      );
    }
    else
    {
      return std::hash<std::remove_cv_t<std::remove_reference_t<A1>>>()(
        std::get<0>(t));
    }
  }

  auto operator()(std::tuple<T...> const& t) const noexcept
  {
    return apply_tuple(t,
      std::make_index_sequence<sizeof...(T) - 1>()
    );
  }
};
    
    /**
     * runtime invocation of function `F` for tuple `T`'s element `i`
     */
    template <typename T, typename F, size_t I = 0>
    static std::invoke_result_t<F, typename std::tuple_element_t<I, T> > 
    select(size_t i, F&& fun)
    {
        using type = typename std::tuple_element_t<I, T>;
        if(i == I)
        {
            type arg;
            return std::forward<F>(fun)(arg);
        }
        if constexpr (I + 1 < std::tuple_size<T>::value)
        {
            select<T, F, I + 1>(i, std::forward<F>(fun));
        }
    }
    
    template <typename T>
    struct flatten_tuple
    {
        using type = std::tuple<T>;
    };

    template <typename T>
    using flatten_tuple_t = typename flatten_tuple<T>::type;
    /**
     * flatten a tuple of tuples, f.eks.: std::tuple<int, std::tuple<float, bool> > becomes std::tuple<int,float,bool>
     */
    template <typename... Ts>
    struct flatten_tuple<std::tuple<Ts...>>{
        using type = decltype(
            std::tuple_cat(std::declval<flatten_tuple_t<Ts>>()...)
        );
    };
    
    //Read the last element of the tuple without calling recursively
    template <std::size_t idx, class... T>
        typename std::enable_if<idx >= std::tuple_size<std::tuple<T...>>::value - 1>::type
        read_tuple(std::istream &in, std::tuple<T...> &out, const char delimiter) {
            std::string cell;
            std::getline(in, cell, delimiter);
            std::stringstream cell_stream(cell);
            cell_stream >> std::get<idx>(out);
        }

    // Read the @p idx-th element of the tuple and then calls itself with @p idx + 1 to
    /// read the next element of the tuple. Automatically falls in the previous case when
    /// reaches the last element of the tuple thanks to enable_if
    template <std::size_t idx, class... T>
        typename std::enable_if<idx < std::tuple_size<std::tuple<T...>>::value - 1>::type
        read_tuple(std::istream &in, std::tuple<T...> &out, const char delimiter) {
            std::string cell;
            std::getline(in, cell, delimiter);
            std::stringstream cell_stream(cell);
            cell_stream >> std::get<idx>(out);
            read_tuple<idx + 1, T...>(in, out, delimiter);
        }

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
