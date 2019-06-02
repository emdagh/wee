#pragma once

#include <wee/weegl.h>
#include <core/vec2.hpp>
#include <core/vec3.hpp>
#include <core/enum_cast.hpp>
#include <tuple>
#include <core/logstream.hpp>

/**
 * trick here was to `inline` the definitions.
 * An inline function is allowed to be defined in multiple translation units.
 */
#define DECLARE_VERTEXATTRIBPTR_IMPL_INT(...) \
    template <> \
    inline void glVertexAttribTPointer<__VA_ARGS__>(GLuint index, GLint size, GLenum type, GLboolean, GLsizei stride, const GLvoid* ptr) { \
        glVertexAttribIPointer(index, size, type, stride, ptr); \
    }

#define DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(...) \
    template <> \
    inline void glVertexAttribTPointer<__VA_ARGS__>(GLuint index, GLint size, GLenum type, GLboolean is_normalized, GLsizei stride, const GLvoid* ptr) { \
        glVertexAttribPointer(index, size, type, is_normalized, stride, ptr); \
    }

namespace wee {

    struct vec4i {
        uint16_t x, y, z, w;
    };
    struct ushort2 {
        uint16_t x, y;
    };

    enum class vertex_stream: uint8_t {
        Position = 0,
        BlendWeight,
        Normal,
        Color0,
        Color1,
        FogCoord,
        PointSize,
        BlendIndex,
        TexCoord0,
        TexCoord1,
        TexCoord2,
        TexCoord3,
        TexCoord4,
        TexCoord5,
        TexCoord6,
        TexCoord7,
        StreamIndexMax
    };

    enum class vertex_stream_type: uint8_t {
        Single,
        Vector2,
        Vector3,
        Vector4,
        HalfSingle,
        HalfVector2,
        HalfVector3,
        HalfVector4,
        Color,
        Short2,
        Short4,
        Short2Normalized,
        Short4Normalized,
        Byte4,
        StreamTypeMax
    };

    static const size_t vertex_streamsize [] = {
        sizeof(float),
        sizeof(float) * 2,
        sizeof(float) * 3,
        sizeof(float) * 4,
        sizeof(int16_t),
        sizeof(int16_t) * 2,
        sizeof(int16_t) * 3,
        sizeof(int16_t) * 4,
        sizeof(uint8_t) * 4, // Color,
        sizeof(int16_t) * 2,
        sizeof(int16_t) * 4,
        sizeof(int16_t) * 2,
        sizeof(int16_t) * 4,
        sizeof(uint8_t) * 4, // Byte4,
    };

    [[maybe_unused]] 
    static const char* vertex_stream_semantic[] = {
        "Position",
        "BlendWeight",
        "Normal",
        "Color0",
        "Color1",
        "FogCoord",
        "PointSize",
        "BlendIndex0",
        "TexCoord0",
        "TexCoord1",
        "TexCoord2",
        "TexCoord3",
        "TexCoord4",
        "TexCoord5",
        "TexCoord6",
        "TexCoord7",
    };

    template <GLenum T>
    void glVertexAttribTPointer(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *) {
        DEBUG_METHOD();
        throw not_implemented();
    }

    

    template <typename... Ts>
    struct vertex : public Ts... {
        enum { kAttributeCount = sizeof...(Ts) };

        template <size_t P>
        using attribute = typename std::tuple_element<P, std::tuple<Ts...> >::type;

        vertex() : Ts()... {}
    };

    template <typename T> 
    struct vertex_attribute_installer
    {
        void operator () (size_t stride, size_t& offset) {
            glVertexAttribTPointer<T::type>(T::semantic, T::size, T::type, T::is_normalized, stride, (GLvoid*)offset);
            glEnableVertexAttribArray(T::semantic);
            offset += sizeof(T); 
        }
    };
    template <typename T, template <typename> typename I, size_t... P>
    void install_vertex_attributes(size_t& offset, const std::index_sequence<P...>&) {
        const size_t stride = sizeof(T);
#if 0
        using expand = int[];
        static_cast<void>(expand{0, (static_cast<void>(
            I<typename T::template attribute<P> >()(stride, offset) // wow...
        ), 0)...}); 
#else
        (I<typename T::template attribute<P> >()(stride, offset), ...);
#endif

    }

    template <typename T, template <typename> class I>
    void install_vertex_attributes() {
        size_t offset = 0;
        install_vertex_attributes<T, I>(offset, std::make_index_sequence<T::kAttributeCount>{});
    }
    struct attributes {

        struct normal {
            vec3f _normal;
            enum {
                size = 3,
                type = GL_FLOAT,
                is_normalized = GL_TRUE,
                semantic = static_cast<int>(vertex_stream::Normal)
            };
        };

        struct blend_index {
            vec4i _index;
            enum {
                size    = 4,
                type    = GL_UNSIGNED_SHORT,
                is_normalized = false,
                semantic = static_cast<int>(vertex_stream::BlendIndex)
            };
        };
        struct position {
            vec3f _position;
            enum {
                size = 3,
                type = GL_FLOAT,
                is_normalized = GL_FALSE,
                semantic = static_cast<int>(vertex_stream::Position)
            };
        };


        struct texcoord {
            vec2f _texcoord;
            enum {
                size = 2,
                type = GL_FLOAT, //GL_UNSIGNED_SHORT,
                is_normalized = GL_FALSE,
                semantic = static_cast<int>(vertex_stream::TexCoord0)
            };
        };

        struct primary_color {
            unsigned int _color;
            enum {
                size = 4,
                type = GL_UNSIGNED_BYTE,
                is_normalized = GL_TRUE,
                semantic = static_cast<int>(vertex_stream::Color0)
            };
        };
        
    };
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_HALF_FLOAT);
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_FLOAT);
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_DOUBLE);
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_FIXED);
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_INT_2_10_10_10_REV);
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_UNSIGNED_INT_2_10_10_10_REV)
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_UNSIGNED_INT_10F_11F_11F_REV);
    
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_BYTE);
    DECLARE_VERTEXATTRIBPTR_IMPL_INT(GL_SHORT);
    DECLARE_VERTEXATTRIBPTR_IMPL_INT(GL_INT);
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_UNSIGNED_BYTE);
    DECLARE_VERTEXATTRIBPTR_IMPL_INT(GL_UNSIGNED_SHORT);
    DECLARE_VERTEXATTRIBPTR_IMPL_INT(GL_UNSIGNED_INT);
}
