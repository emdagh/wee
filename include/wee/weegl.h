#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#define GL_GLEXT_PROTOTYPES 1
#if defined(_MSC_VER)
# include <windows.h>
//# include <gl/GL.h>
#endif
//#include <SDL_opengl.h>
//#include <SDL_opengl_glext.h>
#define BUFFER_OFFSET(i) ((char *)NULL + (i)) 
