#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GL/wglew.h>

//Expose GLFW NATIVE
#define GLFW_EXPOSE_NATIVE_WIN32

//Define GLFW context API macros
#define GLFW_EXPOSE_NATIVE_WGL

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// -------------------------------------------------------------------------
// Check for OpenGL Errors
// Returns 1 if an OpenGL error occurred, 0 otherwise.

#define CheckOpenGLError() GLCheckError(__FILE__, __LINE__)

inline int GLCheckError(char *file, int line)
{
    GLenum glErr;
    int retCode = 0;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        printf("\nOpenGL ERROR => \n\tFILE: %s \n\tLINE: %d\n\tERROR: %d\n", file, line, glErr);
		std::cout << "-----------------------------------------------------\n" << std::endl;
        retCode = 1;
    }
    return retCode;
}