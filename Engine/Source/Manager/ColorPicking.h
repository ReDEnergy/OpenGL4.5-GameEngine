#pragma once

#include <include/dll_export.h>
#include <include/glm.h>

class ColorPicking {

	protected:
		ColorPicking();
		ColorPicking(ColorPicking &copy);
		ColorPicking& operator= (ColorPicking const &copy);
		~ColorPicking();

	public:
		glm::vec3 GetColorUID();
		glm::vec3 colorID;
};

