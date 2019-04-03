#pragma once

#include <gfx/basic_device_buffer.hpp>

namespace wee {

    template <typename T>
    using basic_vertex_buffer = basic_device_buffer<T, GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING>; 
    typedef basic_vertex_buffer<char> vertex_buffer;

}
