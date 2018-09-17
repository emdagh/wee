#pragma once

#include <wee/wee.h>
#include <exception>
#include <stdexcept>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&) = delete;        \
void operator=(TypeName) = delete

namespace { // prevent cluttering of global namespace

    struct not_implemented : std::logic_error {
        not_implemented() : std::logic_error("function or method not implemented") {
        }
    };
}
