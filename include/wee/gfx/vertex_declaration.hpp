#pragma once

#include <core/builder.hpp>

namespace wee {
    struct vertex_declaration : public builder<vertex_declaration> {
        std::vector<vertex_element*> _elements;
        int _stride;

        vertex_declaration& element(const vertex_element&);
        vertex_declaration& stride(int stride);
    };
}
