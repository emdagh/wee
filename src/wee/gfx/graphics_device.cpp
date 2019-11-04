#include <gfx/graphics_device.hpp>
#include <cassert>

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

void GLAPIENTRY glDebugCallback( GLenum source,
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
	glewExperimental = GL_TRUE;
	auto err = glewInit();
    if( err != GLEW_OK) {
        throw std::runtime_error("glew init failed.");//std::string(static_cast<const char*>(glewGetErrorString(err))));
    }
    DEBUG_LOG("vendor:", glGetString(GL_VENDOR));
    DEBUG_LOG(glGetString(GL_RENDERER));
    DEBUG_LOG(glGetString(GL_VERSION));
	if (glGetString(GL_SHADING_LANGUAGE_VERSION)) {
		DEBUG_LOG(glGetString(GL_SHADING_LANGUAGE_VERSION));
	}
    
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_CULL_FACE);
#if 1 //!defined(_MSC_VER)
    glDebugMessageCallback( glDebugCallback, 0 );
#endif

	//glGenVertexArrays(1, &_vao);
}
graphics_device::~graphics_device() {
}



void graphics_device::set_index_buffer(index_buffer* buf) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->_handle);
}

extern "C" int make_graphics_device(graphics_device** p) {
	assert(*p == nullptr);
	*p = new graphics_device(nullptr);
	return 0;
}

extern "C" int delete_graphics_device(graphics_device* p) {
	assert(p != nullptr);
	delete p;
	p = nullptr;
	return 0;
}
