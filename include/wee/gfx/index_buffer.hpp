#pragma once
#if 1
#include <gfx/basic_device_buffer.hpp>

namespace wee {

    enum struct index_type: uint8_t {
        kUnsignedByte,
        kUnsignedShort,
        kUnsignedInt
    };

    template <typename T>
    using basic_index_buffer = basic_device_buffer<T, GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING>; 
    typedef basic_index_buffer<char> index_buffer;

}
#else
namespace wee {
    template <typename T, typename Traits = std::char_traits<T> >
    struct index_buffer {
        using base_type = std::basic_streambuf<T, Traits>;
        using char_type = typename base_type::char_type;
        using int_type  = typename base_type::int_type;

        std::vector<char_type> _buffer;

        index_buffer(size_t n) 
        : _buffer {n}
        {
            glGenBuffers(1, &_handle);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(T) * n, NULL, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            base_type::setp(_buffer.begin(), _buffer.end());
        }

        int_type flush() {
            int n = base_type::pptr() - base_type::pbase();
            //GLvoid* a = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, n, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, n, NULL, GL_STATIC_DRAW);
            if(GLvoid* a = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY); a != nullptr) {
                memcpy(a, _buffer, n);
                glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
            }

        }

        int_type overflow(int c) {
            return base_type::overflow(c);
        }
    };
};
#endif
