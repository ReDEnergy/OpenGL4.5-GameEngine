#include "PhysicsOrb.h"
#include <Manager/Manager.h>
#include <Game/Orbs/LightOrb.h>
#include <include/gl.h>

// Math and base includes
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>

#include <Physics2012/Utilities/Dynamics/KeyFrame/hkpKeyFrameUtility.h>
#include <Physics2012/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h> 
#include <Physics2012/Collide/Shape/Convex/Sphere/hkpSphereShape.h> 


hkpRigidBody* PhysicsOrb::collector = nullptr;

PhysicsOrb::PhysicsOrb(LightOrb *light)
	:Physics(light)
{
	hkpRigidBodyCinfo boxInfo;
	hkpShape* sphereShape = new hkpSphereShape(light->bulbSize.x);

	// Somethings for phantom shape
	hkpBvShape* bvShape = new hkpBvShape(sphereShape, this);

	boxInfo.m_motionType = hkpMotion::MOTION_KEYFRAMED;
	boxInfo.m_shape = bvShape;
	body = new hkpRigidBody( boxInfo );
	AddToWorld();
}

PhysicsOrb::~PhysicsOrb() {
}


void PhysicsOrb::phantomEnterEvent(const hkpCollidable* phantomColl,
								   const hkpCollidable* otherColl,
								   const hkpCollisionInput& env)
{
	hkpRigidBody* owner = hkpGetRigidBody(otherColl);
	if (owner == collector) {
		dynamic_cast<LightOrb*>(parent)->CollectOrb();
	}
}

void PhysicsOrb::phantomLeaveEvent(const hkpCollidable* phantomColl,
								   const hkpCollidable* otherColl)
{
}

void PhysicsOrb::Update() {
	if (body->isActive()) {
		double time = glfwGetTime();
		glm::vec3 pos = basePos;
		glm::vec3 kpos = glm::vec3(pos.x, pos.y + sin(time)/2, pos.z);
		SetPosition(kpos);
	}
}

void PhysicsOrb::SetCollector(GameObject *obj) {
	collector = obj->physics ? obj->physics->body : nullptr;
}
