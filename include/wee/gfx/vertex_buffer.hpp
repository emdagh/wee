#pragma once

#include <gfx/basic_device_buffer.hpp>

namespace wee {

    template <typename T>
    using basic_vertex_buffer = basic_device_buffer<T, GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING>; 
    typedef basic_vertex_buffer<char> vertex_buffer;

    struct with_vertex_buffer {
        GLint _prev;
        with_vertex_buffer(vertex_buffer* b) {
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &_prev);
            glBindBuffer(GL_ARRAY_BUFFER, b->_handle);
        }

        ~with_vertex_buffer() {
            glBindBuffer(GL_ARRAY_BUFFER, _prev);
        }
    };
}
