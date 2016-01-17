//#include <pch.h>
#include "ColorManager.h"
#include <Core/GameObject.h>

typedef unsigned int uint;

ColorManager::ColorManager()
{
	colorID = glm::ivec3(255, 255, 255);
	offset = glm::ivec3(17, 17, 51);
	invOffset = colorID / offset;

	colorMap.resize(invOffset.x + invOffset.y * 16 + invOffset.z * 256);
}

ColorManager::~ColorManager()
{
}

glm::vec3 ColorManager::GetColorUID(GameObject* object)
{
	if (colorID.z) {
		colorID.z -= offset.z;
	} else {
		colorID.z = 255;
		if(colorID.y) {
			colorID.y -= offset.y;
		} else {
			colorID.y = 255;
			colorID.x -= offset.x;
		}
	}

	auto toIndex = colorID / offset;
	auto uid = toIndex.x + toIndex.y * 16 + toIndex.z * 256;

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

GameObject * ColorManager::GetObjectByColor(const glm::vec3 & unitColor) const
{
	auto UID = GetUKeyFromColor(unitColor);
	return colorMap[UID];
}

GameObject * ColorManager::GetObjectByID(unsigned int ID) const
{
	if (ID < colorMap.size())
		return colorMap[ID];
	return nullptr;
}

unsigned int ColorManager::GetUKeyFromColor(const glm::vec3 &unitColor) const
{
	return uint(unitColor.x * invOffset.x) + uint(unitColor.y * invOffset.y) * 16 + uint(unitColor.z * invOffset.z) * 256;
}
