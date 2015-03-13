#pragma once
#include <include/dll_export.h>
#include <include/havok.h>

#include <Component/Physics.h>

// character rigid body
#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>

// Shapes
#include <Physics2012/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics2012/Dynamics/Entity/hkpRigidBodyCinfo.h>

// Phantoms
#include <Physics2012/Dynamics/Entity/hkpEntity.h>
#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>
#include <Physics2012/Collide/Shape/Misc/Bv/hkpBvShape.h>
#include <Physics2012/Collide/Shape/Misc/PhantomCallback/hkpPhantomCallbackShape.h> 


class DLLExport PhysicsPhantom
	: public Physics,
	public hkpPhantomCallbackShape
{
	public:
		PhysicsPhantom(GameObject *obj);
		~PhysicsPhantom();

		void phantomEnterEvent(const hkpCollidable* phantomColl,
		    const hkpCollidable* otherColl, const hkpCollisionInput& env);
		
		void phantomLeaveEvent( const hkpCollidable* phantomColl,
		    const hkpCollidable* otherColl );

		void Update();
		static void SetCollector(GameObject *obj);

	private:
		hkpRigidBody* collidable;
};

