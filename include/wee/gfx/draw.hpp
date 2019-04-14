#pragma once

#include <weegl.h>
#include <gfx/primitive_type.hpp>

#define OPENGL_DEFINE_PRIMITIVE(x, T) \
    template <> struct __primitive__<static_cast<int>(x)> { \
        static const GLenum type = T; \
    };
#define OPENGL_DEFINE_INDEX(x, T) \
    template <> struct __index__<static_cast<int>(x)> { \
        static const GLenum type = T; \
    };

namespace wee {
    template <int P> struct __primitive__ { 
        static const GLenum type = GL_NONE; 
    };
    template <int P> struct __index__{ 
        static const GLenum type = GL_NONE; 
    };

    OPENGL_DEFINE_PRIMITIVE(primitive_type::kPoints,         GL_POINTS); 
    OPENGL_DEFINE_PRIMITIVE(primitive_type::kLineStrip,      GL_LINE_STRIP);
    OPENGL_DEFINE_PRIMITIVE(primitive_type::kLineLoop,       GL_LINE_LOOP);
    OPENGL_DEFINE_PRIMITIVE(primitive_type::kLineList,       GL_LINES);
    OPENGL_DEFINE_PRIMITIVE(primitive_type::kTriangleStrip,  GL_TRIANGLE_STRIP);
    OPENGL_DEFINE_PRIMITIVE(primitive_type::kTriangleFan,    GL_TRIANGLE_FAN);
    OPENGL_DEFINE_PRIMITIVE(primitive_type::kTriangles,      GL_TRIANGLES);
    //OPENGL_DEFINE_TYPE(primitive_type::k###, GL_LINE_STRIP_ADJACENCY);
    //OPENGL_DEFINE_TYPE(primitive_type::k###, GL_LINES_ADJACENCY);
    //OPENGL_DEFINE_TYPE(primitive_type::k###, GL_TRIANGLE_STRIP_ADJACENCY);
    //OPENGL_DEFINE_TYPE(primitive_type::k###, GL_TRIANGLES_ADJACENCY);
    //OPENGL_DEFINE_TYPE(primitive_type::k###, GL_PATCHES);
    OPENGL_DEFINE_INDEX(index_type::kUnsignedByte,   GL_UNSIGNED_BYTE);
    OPENGL_DEFINE_INDEX(index_type::kUnsignedShort,  GL_UNSIGNED_SHORT);
    OPENGL_DEFINE_INDEX(index_type::kUnsignedInt,    GL_UNSIGNED_INT);

    template <primitive_type _Primitive>
    void draw_primitives(int first, size_t count) {
        glDrawArrays(__primitive__<static_cast<int>(_Primitive)>::type, first, count);
    }

    template <primitive_type _Primitive, index_type _Index>
    void draw_indexed_primitives(size_t count, int baseVertex) {
        glDrawElementsBaseVertex(
            __primitive__<static_cast<int>(_Primitive)>::type,
            count,
            __index__<static_cast<int>(_Index)>::type,
            BUFFER_OFFSET(0),
            baseVertex
        );
    }
}
