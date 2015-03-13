//#include <pch.h>
#include "ColorPicking.h"


ColorPicking::ColorPicking() {
	colorID = glm::ivec3(255, 255, 255);
}

ColorPicking::~ColorPicking() {
}

glm::vec3 ColorPicking::GetColorUID()
{
	if (colorID.z) {
		colorID.z -= 1;
	} else {
		colorID.z = 255;
		if(colorID.y) {
			colorID.y -= 1;
		} else {
			colorID.y = 255;
			colorID.x -= 1;
		}
	}
	glm::vec3 x = glm::vec3(colorID / 255.0f);
	return x;
}


/*
void World::pickObject(int x, int y) {

	// get color information from frame buffer
	unsigned char pixel[3];
	GLint viewport[4];

	glDisable(GL_LIGHTING);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	drawSelectable();

	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	glEnable(GL_LIGHTING);

	Vector3D color = Vector3D(pixel[0], pixel[1], pixel[2]);
	getSelected(color);
}
*/