#pragma once

#include <include/dll_export.h>
#include <include/glm.h>
#include <include/havok.h>

#include <Core/Object.h>

#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>

class GameObject;

class DLLExport Physics: virtual public Object {
	public:
		Physics(GameObject *parent);
		Physics(Physics &physics);
		Physics::~Physics();

		void LoadHavokFile(const char *fileName);
		virtual void Update();
		virtual void AddToWorld();
		virtual void RemoveFromWorld();
		virtual void Deactivate();
		virtual void SetPosition(glm::vec3 pos);

	protected:
		GameObject *parent;

	public:
		hkpRigidBody *body;
};