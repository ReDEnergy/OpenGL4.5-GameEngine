#pragma once

#include <include/dll_export.h>
#include <include/glm.h>
#include <memory>
#include <vector>

class GameObject;

class DLLExport ColorManager
{
	protected:
		ColorManager();
		~ColorManager();

	public:
		glm::vec3 GetColorUID(GameObject* object);
		glm::ivec3 GetOffsetID() const;
		glm::ivec3 GetChannelsEncodeSize() const;
		GameObject* GetObjectByColor(const glm::vec3 &unitColor) const;
		GameObject* GetObjectByID(unsigned int ID) const;

		// color components must be in [0 - 1] range
		unsigned int GetUKeyFromColor(const glm::vec3 &unitColor) const;

	private:
		
		glm::ivec3 colorID;
		glm::ivec3 offset;
		glm::ivec3 invOffset;
		std::vector<GameObject*> colorMap;
};

