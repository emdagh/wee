#pragma once

#include <weegl.h>
#include <gfx/primitive_type.hpp>
#include <gfx/index_buffer.hpp>
/*#define DECLARE_PRIMITIVE(a, b) \
    template <> \
    struct glGetPrimitiveType<primitive_type::a> { \
        static const GLenum type = b;\
    }*/

#define OPENGL_DEFINE_PRIMITIVE(x, T) \
    template <> struct glGetPrimitiveType<x> { \
        static const GLenum type = T; \
    };
#define OPENGL_DEFINE_INDEX(x, T) \
    template <> struct glGetIndexType<x> { \
        static const GLenum type = T; \
    };

namespace wee {
    template <primitive_type T>
    struct glGetPrimitiveType {
        static const GLenum type = GL_NONE;
    };
    template <index_type P> struct glGetIndexType{ 
        static const GLenum type = GL_NONE; 
    };

    OPENGL_DEFINE_PRIMITIVE(primitive_type::points,         GL_POINTS); 
    OPENGL_DEFINE_PRIMITIVE(primitive_type::line_strip,      GL_LINE_STRIP);
    OPENGL_DEFINE_PRIMITIVE(primitive_type::line_loop,       GL_LINE_LOOP);
    OPENGL_DEFINE_PRIMITIVE(primitive_type::line_list,       GL_LINES);
    OPENGL_DEFINE_PRIMITIVE(primitive_type::triangle_strip,  GL_TRIANGLE_STRIP);
    OPENGL_DEFINE_PRIMITIVE(primitive_type::triangle_fan,    GL_TRIANGLE_FAN);
    OPENGL_DEFINE_PRIMITIVE(primitive_type::triangles,      GL_TRIANGLES);
    OPENGL_DEFINE_PRIMITIVE(primitive_type::quads,      GL_QUADS);
    //OPENGL_DEFINE_TYPE(primitive_type::k###, GL_LINE_STRIP_ADJACENCY);
    //OPENGL_DEFINE_TYPE(primitive_type::k###, GL_LINES_ADJACENCY);
    //OPENGL_DEFINE_TYPE(primitive_type::k###, GL_TRIANGLE_STRIP_ADJACENCY);
    //OPENGL_DEFINE_TYPE(primitive_type::k###, GL_TRIANGLES_ADJACENCY);
    //OPENGL_DEFINE_TYPE(primitive_type::k###, GL_PATCHES);
    OPENGL_DEFINE_INDEX(index_type::unsigned_byte,   GL_UNSIGNED_BYTE);
    OPENGL_DEFINE_INDEX(index_type::unsigned_short,  GL_UNSIGNED_SHORT);
    OPENGL_DEFINE_INDEX(index_type::unsigned_int,    GL_UNSIGNED_INT);
#if 0
    template <primitive_type _Primitive>
    void draw_primitives(int first, size_t count) {
        glDrawArrays(glGetPrimitiveType<_Primitive>::type, first, count);
    }

    template <primitive_type _Primitive, index_type _Index>
    void draw_indexed_primitives(size_t count, int baseVertex) {
        glDrawElementsBaseVertex(
            glGetPrimitiveType<_Primitive>::type,
            count,
            glGetIndexType<_Index>::type,
            BUFFER_OFFSET(0),
            baseVertex
        );
    }
#endif
}
