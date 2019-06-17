#pragma once

#define GL_GLEXT_PROTOTYPES 1
#if defined(_MSC_VER)
#include <Windows.h>
#include <gl/GL.h>
#elif !defined(__MACOSX__)
#include <GL/gl.h>
#include <GL/glext.h>
#else
#include <OpenGL/OpenGL.h>
#endif
#define BUFFER_OFFSET(i) ((char *)NULL + (i)) 
