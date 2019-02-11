#pragma once


namespace wee {
    enum class kVertexStreamIndex : uint8_t {
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

    enum class kVertexStreamType : uint8_t {
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

    static const char* kVertexStreamSemantic[] = {
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
    void glVertexAttribTPointer(GLuint,GLint,GLenum,GLboolean,GLsizei,const GLvoid *) {
        throw not_implemented();
    }
    /*template <>
    void glVertexAttribTPointer<GL_FLOAT>(GLuint index, GLint size, GLenum type, GLboolean is_normalized, GLsizei stride, const GLvoid* ptr) {
        glVertexAttribPointer(index, size, type, is_normalized, stride, ptr);
    }*/
#define DECLARE_VERTEXATTRIBPTR_IMPL_INT(...) \
    template <> \
    void glVertexAttribTPointer<__VA_ARGS__>(GLuint index, GLint size, GLenum type, GLboolean, GLsizei stride, const GLvoid* ptr) { \
        glVertexAttribIPointer(index, size, type, stride, ptr); \
    }

#define DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(...) \
    template <> \
    void glVertexAttribTPointer<__VA_ARGS__>(GLuint index, GLint size, GLenum type, GLboolean is_normalized, GLsizei stride, const GLvoid* ptr) { \
        glVertexAttribPointer(index, size, type, is_normalized, stride, ptr); \
    }

    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_HALF_FLOAT);
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_FLOAT);
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_DOUBLE);
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_FIXED);
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_INT_2_10_10_10_REV);
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_UNSIGNED_INT_2_10_10_10_REV)
    DECLARE_VERTEXATTRIBPTR_IMPL_FLOAT(GL_UNSIGNED_INT_10F_11F_11F_REV);
    
    DECLARE_VERTEXATTRIBPTR_IMPL_INT(GL_BYTE);
    DECLARE_VERTEXATTRIBPTR_IMPL_INT(GL_SHORT);
    DECLARE_VERTEXATTRIBPTR_IMPL_INT(GL_INT);
    DECLARE_VERTEXATTRIBPTR_IMPL_INT(GL_UNSIGNED_BYTE);
    DECLARE_VERTEXATTRIBPTR_IMPL_INT(GL_UNSIGNED_SHORT);
    DECLARE_VERTEXATTRIBPTR_IMPL_INT(GL_UNSIGNED_INT);
    

    template <typename... Ts>
    struct vertex : public Ts... {
        enum { kAttributeCount = sizeof...(Ts) };

        template <size_t P>
        using attribute = typename std::tuple_element<P, std::tuple<Ts...> >::type;

        vertex() : Ts()... {}
    };

    template <typename T> // , typename = std::enable_if_t<T::type == GL_FLOAT> > // g++ segfaults here....
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
        using expand = int[];
        const size_t stride = sizeof(T);

        static_cast<void>(expand{0, (static_cast<void>(
            I<typename T::template attribute<P> >()(stride, offset) // wow...
        ), 0)...}); 
    }

    template <typename T, template <typename> class I>
    void install_vertex_attributes() {
        size_t offset = 0;
        install_vertex_attributes<T, I>(offset, std::make_index_sequence<T::kAttributeCount>{});
    }
}
