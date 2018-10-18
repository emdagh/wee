#pragma once

#include <utility>

namespace wee {
    template <typename T, typename... Args>
    void cleanup(T* t, Args&&... args) {
        cleanup(t);
        cleanup(std::forward<Args>(args)...);
    }
}

/**
 * now go ahead and specialize!
 */
