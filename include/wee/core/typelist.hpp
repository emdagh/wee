#pragma once

namespace wee {
  #include <vector>
#include <cstdint>
#include <tuple>
#include <iostream>

template <typename... Ts>
struct type_list
{
    constexpr static std::size_t count{sizeof...(Ts)};
};

template<typename>
constexpr std::size_t locate(std::size_t ind) {
    return static_cast<std::size_t>(-1);
}

template<typename IndexedType, typename T, typename... Ts>
constexpr std::size_t locate(std::size_t ind = 0) {
    if (std::is_same<IndexedType, T>::value) {
        return ind;
    } else {
        return locate<IndexedType, Ts...>(ind + 1);
    }
}
template<typename T, typename>
struct index_of;

template<typename T, typename... ListedTypes>
struct index_of<T, type_list<ListedTypes...>>
  : std::integral_constant<std::size_t, locate<T, ListedTypes...>()>
{
};

template <typename> struct to_container;
// Specialize it, in order to drill down into the template parameters.
template<template<typename...Args> typename t, typename ...Ts>
struct to_container<t<Ts...>> {
    using type = std::tuple<std::vector<Ts>...>;
};
}
