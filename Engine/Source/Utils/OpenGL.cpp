//#include <pch.h>
#include "OpenGL.h"
#include <include/gl.h>

void OpenGL::DrawCircle(float cx, float cy, float r, int num_segments)
{
	float theta = 2 * 3.1415926 / float(num_segments);
	float c = cosf(theta);//precalculate the sine and cosine
	float s = sinf(theta);
	float t;

	float x = r;//we start at angle = 0 
	float y = 0;

	glBegin(GL_LINE_LOOP);
	for (int ii = 0; ii < num_segments; ii++)
	{
		glVertex2f(x + cx, y + cy);	//output vertex 
									//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	glEnd();
}

void OpenGL::DrawLine(glm::vec3 posA, glm::vec3 posB)
{
	glBegin(GL_LINES);
	glVertex3f(posA.x, posA.y, posA.z);
	glVertex3f(posB.x, posB.y, posB.z);
	glEnd();
}

void OpenGL::DrawPolygon(vector<glm::vec3> &vertices) {
	glBegin(GL_LINES);
	// Draw polygon edges
	for (auto vertex : vertices)
		glVertex3f(vertex.x, vertex.y, vertex.z);
	// Close polygon
	glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
	glEnd();
}