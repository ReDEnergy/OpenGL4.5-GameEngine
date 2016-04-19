#pragma once

#include <include/gl.h>

//Expose GLFW NATIVE FUNCTIONALITY
#ifdef _WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
	#define GLFW_EXPOSE_NATIVE_WGL
#elif __linux__
	#define GLFW_EXPOSE_NATIVE_X11
	#define GLFW_EXPOSE_NATIVE_GLX
#endif

#include <GLFW/glfw3native.h>
