#pragma once

#include <cstdlib>

namespace wee {
    template <size_t First, size_t Last, typename Fn>
    inline constexpr void static_for(Fn&& fn) {
        if constexpr(First < Last) {
            fn(First);
            static_for<First + 1, Last>(std::forward<Fn>(fn));
        }
    }
}
