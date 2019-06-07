#include <math.h>
#include <iostream>
#include <bitset>
#include <core/ndview.hpp>
#include <functional>
#include <tuple>
#include <prettyprint.hpp>

using namespace wee;

/*template <typename T>
void ndprint(T& t) {
    auto fun = make_ndfunc([] (auto& el) { std::cout << el << std::endl; }, t); 
    recursive_for<1>(fun);
}*/

int main(int, char**) {

    auto ix = make_ndindexer(4,3,2);
    ix.recursive_for<2>([&ix] (auto... a) { 
        std::cout << std::make_tuple(a...) << std::endl; 
    });
    std::cout << std::endl;

    return 0;
}
