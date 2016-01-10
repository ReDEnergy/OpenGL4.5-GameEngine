#pragma once
#include <list>

#include <include/glm.h>
#include <include/dll_export.h>

#include <Core/GameObject.h>

class DLLExport ControlObject : virtual public GameObject
{
	public:
		ControlObject();
		~ControlObject();
};