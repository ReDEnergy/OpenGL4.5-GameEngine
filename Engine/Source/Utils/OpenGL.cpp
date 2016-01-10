//#include <pch.h>
#include "OpenGL.h"
#include <include/gl.h>
#include <include/math.h>

#include <Core/Engine.h>
#include <Core/WindowObject.h>

void OpenGL::DrawCircle(const glm::vec3 &center, float radius, int num_segments)
{
	if (Engine::Window->IsCoreContext()) return;

	float theta = 2 * 3.1415926f / float(num_segments);
	
	//precalculate the sine and cosine
	float c = cosf(theta);
	float s = sinf(theta);
	float t;

	// Start at angle = 0 
	float x = radius;
	float y = 0;

	glBegin(GL_LINE_LOOP);
	for (int ii = 0; ii < num_segments; ii++)
	{
		// Output vertex 
		glVertex3f(x + center.x, center.y, y + center.z);
		
		// Apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	glEnd();
	CheckOpenGLError();
}

void OpenGL::DrawCone(const glm::vec3 &center, float radius, float height, int num_segments)
{
	if (Engine::Window->IsCoreContext()) return;

	float theta = 2 * 3.1415926f / float(num_segments);

	//precalculate the sine and cosine
	float c = cosf(theta);
	float s = sinf(theta);
	float t;

	// Start at angle = 0 
	float x = radius;
	float y = 0;

	glBegin(GL_LINE_LOOP);
	for (int ii = 0; ii < num_segments; ii++)
	{
		// Output vertex 
		glVertex3f(x + center.x, center.y, y + center.z);

		// Apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	for (int ii = 0; ii < num_segments; ii++)
	{
		// Output vertex 
		glVertex3f(center.x, center.y + height, center.z);
		glVertex3f(x + center.x, center.y, y + center.z);

		// Apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	glEnd();
	CheckOpenGLError();
}

void OpenGL::DrawLine(const glm::vec3 &pos)
{
	if (Engine::Window->IsCoreContext()) return;

	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(pos.x, pos.y, pos.z);
	glEnd();
	CheckOpenGLError();
}

void OpenGL::DrawLine(const glm::vec3 &posA, const glm::vec3 &posB)
{
	if (Engine::Window->IsCoreContext()) return;

	glBegin(GL_LINES);
	glVertex3f(posA.x, posA.y, posA.z);
	glVertex3f(posB.x, posB.y, posB.z);
	glEnd();
	CheckOpenGLError();
}

void OpenGL::DrawPolygon(vector<glm::vec3> &vertices)
{
	if (Engine::Window->IsCoreContext()) return;

	glBegin(GL_LINES);
	// Draw polygon edges
	for (auto vertex : vertices)
		glVertex3f(vertex.x, vertex.y, vertex.z);
	// Close polygon
	glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
	glEnd();
	CheckOpenGLError();
}

DLLExport void OpenGL::DispatchCompute(uint sizeX, uint sizeY, uint sizeZ, uint workGroupSize, bool synchronize)
{
	glDispatchCompute(GLuint(UPPER_BOUND(sizeX, workGroupSize)), GLuint(UPPER_BOUND(sizeY, workGroupSize)), GLuint(UPPER_BOUND(sizeZ, workGroupSize)));
	if (synchronize) {
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}
	CheckOpenGLError();
}