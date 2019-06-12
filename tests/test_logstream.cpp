#include <math.h>
#include <iostream>
#include <bitset>
#include <core/ndview.hpp>
#include <core/array.hpp>
#include <functional>
#include <tuple>
#include <prettyprint.hpp>

using namespace wee;

/*template <typename T, size_t N, T V>
std::array<T, N> make_array(const std::array<T, N>& ary) {
    //auto _ary = ary;
    if constexpr(sizeof...(Ts) < N) {
        return make_array<T, N, Value>(ts..., Value);
    }
    return std::array<T,N> { ts... };
    
}*/

/*template <typename T>
void ndprint(T& t) {
    auto fun = make_ndfunc([] (auto& el) { std::cout << el << std::endl; }, t); 
    recursive_for<1>(fun);
}*/




int main(int, char**) {

    std::vector<int> test(4 * 3 * 2);
    std::iota(test.begin(), test.end(), 0);

    auto ix = make_ndindexer(2,3,4);

    ix.submatrix(1, {2,3,1}, [&] (auto i) {
        std::cout << (i) << std::endl;
    });

    auto x = wee::make_array<ptrdiff_t, 3, 1>(2,3);
    std::cout << x << std::endl;

    ix.recursive_for<3>([&ix] (auto a) { 
        std::cout <<a<< std::endl; 
    }, 1, { 2, 3, 1 });//ix.shape());
    std::cout << std::endl;

    return 0;
}
