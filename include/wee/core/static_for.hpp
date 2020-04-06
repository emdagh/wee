#pragma once

#include <cstdlib>

namespace wee {
    template <size_t First, size_t Last, typename Fn>
    inline void static_for(const Fn& fn) {
        if constexpr(First < Last) {
            fn(First);
            static_for<First + 1, Last>(fn);
        }
    }
}
