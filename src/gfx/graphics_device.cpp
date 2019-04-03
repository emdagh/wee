#include <gfx/graphics_device.hpp>

using namespace wee;

template <typename T>
void glGet(GLenum pname, T* data) {
    throw not_implemented();
}

#define DECLARE_GLGET_TYPE(T, fn) template <> void glGet<T>(GLenum pname, T* data) { fn(pname, data); }

DECLARE_GLGET_TYPE(GLboolean,   glGetBooleanv);
DECLARE_GLGET_TYPE(GLdouble,    glGetDoublev);
DECLARE_GLGET_TYPE(GLfloat,     glGetFloatv);
DECLARE_GLGET_TYPE(GLint,       glGetIntegerv);
DECLARE_GLGET_TYPE(GLint64,     glGetInteger64v);

void GLAPIENTRY
glDebugCallback( GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam )
{
    if(severity == GL_DEBUG_SEVERITY_HIGH) {
        throw std::runtime_error(message);
    }
}

graphics_device::graphics_device(SDL_Renderer* renderer) 
: _renderer(renderer) 
{
    
    DEBUG_LOG("vendor:", glGetString(GL_VENDOR));
    DEBUG_LOG(glGetString(GL_RENDERER));
    DEBUG_LOG(glGetString(GL_VERSION));
    DEBUG_LOG(glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_CULL_FACE);

    glDebugMessageCallback( glDebugCallback, 0 );
}
