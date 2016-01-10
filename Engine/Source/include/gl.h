#pragma once
#include <GL/glew.h>
#include <GL/wglew.h>

//Expose GLFW NATIVE
#define GLFW_EXPOSE_NATIVE_WIN32

//Define GLFW context API macros
#define GLFW_EXPOSE_NATIVE_WGL

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <include/dll_export.h>

// -------------------------------------------------------------------------
// Check for OpenGL Errors
// Returns 1 if an OpenGL error occurred, 0 otherwise.

#ifdef _DEBUG
#define CheckOpenGLError() OpenGL::CheckError(__FILE__, __LINE__)
#elif
#define CheckOpenGLError() ;
#endif


namespace OpenGL
{
	DLLExport int CheckError(const char *file, int line);
};
