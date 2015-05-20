#pragma once

#include <include/dll_export.h>
#include <include/glm.h>

class ColorManager {

	protected:
		ColorManager();
		ColorManager(ColorManager &copy);
		ColorManager& operator= (ColorManager const &copy);
		~ColorManager();

	public:
		glm::vec3 GetColorUID();
		glm::vec3 colorID;
};

