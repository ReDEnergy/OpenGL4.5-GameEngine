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
		unsigned int GetObjectUID(GameObject* object);
		glm::vec3 GetColorUID(GameObject* object);
		glm::ivec3 GetOffsetID() const;
		glm::ivec3 GetChannelsEncodeSize() const;
		GameObject* GetObjectByColor(const glm::ivec3 &color) const;
		GameObject* GetObjectByID(unsigned int ID) const;

	private:
		
		glm::ivec3 colorID;
		glm::ivec3 offset;
		std::vector<GameObject*> colorMap;
};

