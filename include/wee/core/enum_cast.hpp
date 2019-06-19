#pragma once

#include <type_traits>

namespace wee {


    template <typename E>
    constexpr typename std::underlying_type<E>::type enum_cast(E x) {
        return static_cast<typename std::underlying_type<E>::type>(x);
    }

}
