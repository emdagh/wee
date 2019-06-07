#include <math.h>
#include <iostream>
#include <bitset>
#include <core/ndview.hpp>
#include <functional>

using namespace wee;

template <typename T>
void ndprint(T&& t) {
    std::function<void(T&&)> fn = make_ndfunc([] (auto el) { 
            std::cout << el << std::endl; 
        },
        std::forward<T>(t)
    );
}

int main(int, char**) {
    ndindexer<3> ix({4,3,2});

    ndprint(ix);
   

    return 0;
}
