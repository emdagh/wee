#include <gfx/vertex_declaration.hpp>
#if 0
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

namespace wee {
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
}
#endif
