#pragma once

namespace wee {
    // https://stackoverflow.com/questions/40380447/python-like-map-in-c
    template<template<class...>class Z, class C, class Op>
        struct calculate_return_type {
            using dC = typename std::decay<C>::type;
            using T_in = typename dC::reference;
            using T_out = typename std::decay< typename std::result_of< Op&(T_in) >::type >::type;
            using R = Z<T_out>;
        };

    template <template<class...>class Z=std::vector, class C, class UnaryOperator>
        auto map(C&& c_in, UnaryOperator&& operation)
        -> typename calculate_return_type<Z, C, UnaryOperator>::R
        {
            using R = typename calculate_return_type<Z, C, UnaryOperator>::R;
            R result;
            result.reserve(c_in.size());
            using T_in = typename calculate_return_type<Z, C, UnaryOperator>::T_in;

            using std::begin; using std::end;
            std::transform(
                    begin(c_in), end(c_in),
                    std::back_inserter(result),
                    [&] (T_in item) { return operation(decltype(item)(item)); }
                    );
            return result;
        }
}
