#pragma once
#include "gl.h"

#include <iostream>

using namespace std;

static char *errorDescription[8] = {
	"GL_INVALID_ENUM",						// 0x0500
	"GL_INVALID_VALUE",						// 0x0501
	"GL_INVALID_OPERATION",					// 0x0502
	"GL_STACK_OVERFLOW",					// 0x0503
	"GL_STACK_UNDERFLOW",					// 0x0504
	"GL_OUT_OF_MEMORY",						// 0x0505
	"GL_INVALID_FRAMEBUFFER_OPERATION" };	// 0x0506

const char * GetErrorDescription(unsigned int index)
{
	return errorDescription[index];
}

int OpenGL::CheckError(const char * file, int line)
{
	#ifdef _DEBUG
	GLenum glErr = glGetError();
	if (glErr != GL_NO_ERROR)
	{
		cout << "\n[OpenGL Error]\n";
		cout << "\t[" << glErr << "] : " << GetErrorDescription(glErr - 0x500) << "\n";
		cout << "\t[File] : " << file;
		cout << "\t[Line] : " << line << endl;
		return 1;
	}
	#endif
	return 0;
}

