#pragma once

#include <cmath>

namespace wee {

    struct math {

        constexpr static int log2(int n) {
            return ( (n<2) ? 0 : 1 + log2(n>>1));
        }

        template <typename T>
        static T npot(const T& n) {
            return 1 << (log2(n - 1) + 1);
        }
    };
}
