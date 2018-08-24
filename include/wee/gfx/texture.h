#pragma once

#include <core/builder.hpp>

namespace wee {

    template <typename T>
    struct texture : public builder<texture> {
        /*surface_format _format;
        T& format(const surface_format& val) {
            _format = val;
            return static_cast<T&>(*this);
        }*/
    };

    struct texture2d : public texture<texture2d> {

        texture2d& width(int);
        texture2d& height(int);
    };
}
