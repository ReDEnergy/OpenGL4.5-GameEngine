#pragma once
#include <include/dll_export.h>
#include <include/glm.h>

#include <Core/GameObject.h>

enum class GIZMO_MODE {
	TRANSLATE,
	ROTATE,
	SCALE
};

class DLLExport Gizmo : virtual public GameObject
{
	public:
		Gizmo();
		~Gizmo();

		void SetScale(glm::vec3 scale);
		void SetMode(GIZMO_MODE mode);

};