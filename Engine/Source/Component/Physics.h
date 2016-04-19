#pragma once
#include <string>

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

		void LoadHavokFile(const std::string &fileName);
		virtual void Update();
		virtual void AddToWorld();
		virtual void RemoveFromWorld();
		virtual void Deactivate();

		void UpdatePhysicsEngine();
		void SetLinearVelocity(glm::vec3 velocity);

	protected:
		GameObject *parent;

	public:
		hkpRigidBody *body;
};