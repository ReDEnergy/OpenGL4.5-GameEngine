//#include <pch.h>
#include "PhysicsPhantom.h"

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>
#include <Component/Physics.h>

// Math and base includes
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>

#include <Physics2012/Utilities/Dynamics/KeyFrame/hkpKeyFrameUtility.h>
#include <Physics2012/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h> 
#include <Physics2012/Collide/Shape/Convex/Box/hkpBoxShape.h> 


PhysicsPhantom::PhysicsPhantom(GameObject *obj)
	: Physics(obj)
{
	hkpRigidBodyCinfo boxInfo;
	hkpShape* boxShape = new hkpBoxShape(hkVector4(0.2f, 1.5f, 0.5f));

	// Somethings for phantom shape
	hkpBvShape* bvShape = new hkpBvShape(boxShape, this);

	boxInfo.m_motionType = hkpMotion::MOTION_FIXED;
	boxInfo.m_shape = bvShape;
	body = new hkpRigidBody( boxInfo );
}

PhysicsPhantom::~PhysicsPhantom() {
}


void PhysicsPhantom::phantomEnterEvent(const hkpCollidable* phantomColl,
								   const hkpCollidable* otherColl,
								   const hkpCollisionInput& env)
{
	hkpRigidBody* owner = hkpGetRigidBody(otherColl);
	//Manager::Event->EmitAsync(EventType::LEVEL_TRANISTION_GATE, nullptr);
}

void PhysicsPhantom::phantomLeaveEvent(const hkpCollidable* phantomColl,
								   const hkpCollidable* otherColl)
{
}

void PhysicsPhantom::Update() {
}