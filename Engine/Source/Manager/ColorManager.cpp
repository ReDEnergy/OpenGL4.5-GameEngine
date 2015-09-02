//#include <pch.h>
#include "ColorManager.h"


ColorManager::ColorManager() {
	colorID = glm::ivec3(255, 255, 255);
}

ColorManager::~ColorManager() {
}

glm::vec3 ColorManager::GetColorUID()
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