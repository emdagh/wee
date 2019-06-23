#pragma once
#if defined(_MSC_VER)
# include <windows.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#define GLEW_STATIC
#include <GL/glew.h>


#define BUFFER_OFFSET(i) ((char *)NULL + (i)) 
