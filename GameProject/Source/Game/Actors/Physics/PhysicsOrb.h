#pragma once
#include <Game/Orbs/LightOrb.h>
#include <Component/Physics.h>

// character righid body
#include <include/havok.h>
#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>

// Shapes
#include <Physics2012/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics2012/Dynamics/Entity/hkpRigidBodyCinfo.h>

// Phantoms
#include <Physics2012/Dynamics/Entity/hkpEntity.h>
#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>
#include <Physics2012/Collide/Shape/Misc/Bv/hkpBvShape.h>
#include <Physics2012/Collide/Shape/Misc/PhantomCallback/hkpPhantomCallbackShape.h> 


class PhysicsOrb
	: public Physics,
	public hkpPhantomCallbackShape
{
	public:
		PhysicsOrb(LightOrb *light);
		~PhysicsOrb();

		void phantomEnterEvent(const hkpCollidable* phantomColl,
		    const hkpCollidable* otherColl, const hkpCollisionInput& env);
		
		void phantomLeaveEvent( const hkpCollidable* phantomColl,
		    const hkpCollidable* otherColl );

		void Update();
		static void SetCollector(GameObject *obj);

	public:
		glm::vec3 basePos;

	private:
		static hkpRigidBody* collector;
};

