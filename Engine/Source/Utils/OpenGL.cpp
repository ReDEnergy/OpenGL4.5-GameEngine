#include "OpenGL.h"
#include <include/gl.h>
#include <include/math.h>

#include <Core/Engine.h>
#include <Core/WindowObject.h>

static inline GLuint NumGroupSize(int dataSize, int groupSize)
{
	return (dataSize + groupSize - 1) / groupSize;
}

DLLExport void OpenGL::DispatchCompute(uint sizeX, uint sizeY, uint sizeZ, uint workGroupSize, bool synchronize)
{
	glDispatchCompute(NumGroupSize(sizeX, workGroupSize), NumGroupSize(sizeY, workGroupSize), NumGroupSize(sizeZ, workGroupSize));
	if (synchronize) {
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}
	CheckOpenGLError();
}