#include "ColorManager.h"
#include <Core/GameObject.h>

typedef unsigned int uint;

ColorManager::ColorManager()
{
	colorID = glm::ivec3(255, 255, 255);
	offset = glm::ivec3(1, 1, 1);

	auto size = colorID / offset;
	colorMap.resize(size.x + size.y * 16 + size.z * 256);
}

ColorManager::~ColorManager()
{
}

unsigned int ColorManager::GetObjectUID(GameObject * object)
{
	auto uid = colorID.x / offset.x + colorID.y * 16 / offset.y + colorID.z * 256 / offset.z;
	return uid;
}

glm::vec3 ColorManager::GetColorUID(GameObject* object)
{
	if (colorID.z > 0) {
		colorID.z -= offset.z;
	} else {
		colorID.z = 255;
		if(colorID.y > 0) {
			colorID.y -= offset.y;
		} else {
			colorID.y = 255;
			if (colorID.x > 0) {
				colorID.x -= offset.x;
			}
		}
	}

	auto uid = colorID.x / offset.x + colorID.y * 16 / offset.y + colorID.z * 256 / offset.z;
	colorMap[uid] = object;
	return glm::vec3(colorID) / 255.0f;
}

glm::ivec3 ColorManager::GetOffsetID() const
{
	return offset;
}

glm::ivec3 ColorManager::GetChannelsEncodeSize() const
{
	return glm::ivec3(255 / offset.x, 255 / offset.y, 255 / offset.z);
}

GameObject* ColorManager::GetObjectByColor(const glm::ivec3 &color) const
{
	unsigned int UID = color[0] / offset.x  + color[1] * 16 / offset.y + color[2] * 256 / offset.z;
	if (UID < colorMap.size()) {
		return colorMap[UID];
	}
	else {
		printf("[ColorManager][ERROR] color ID out of range\n");
		return nullptr;
	}
}

GameObject* ColorManager::GetObjectByID(unsigned int ID) const
{
	if (ID < colorMap.size())
		return colorMap[ID];

	printf("[ColorManager][ERROR] color ID out of range\n");
	return nullptr;
}
