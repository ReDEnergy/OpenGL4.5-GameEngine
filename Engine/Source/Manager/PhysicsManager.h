#pragma once
#include <include/havok.h>
#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>

class PhysicsManager
{
	protected:
		PhysicsManager();
		~PhysicsManager();

	public:
		hkpRigidBody* GetFirstRigidBody(const char* const fileName);
		hkpRigidBody* GetBox(float X, float Y, float Z);
		hkpRigidBody* GetCopyOf(const hkpRigidBody *body);
};