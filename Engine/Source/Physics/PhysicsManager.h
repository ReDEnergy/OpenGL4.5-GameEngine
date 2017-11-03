#pragma once
#ifdef PHYSICS_ENGINE

#include <include/dll_export.h>

class PhysXCore;
class GameObject;

class DLLExport PhysicsManager
{
	protected:
		PhysicsManager();
		~PhysicsManager();

	public:
		PhysXCore* GetPhysX() const;
		void AddObject(GameObject *obj);
		void RemoveObject(GameObject *obj);

	private:
		PhysXCore* phsyXCore;
};

#endif