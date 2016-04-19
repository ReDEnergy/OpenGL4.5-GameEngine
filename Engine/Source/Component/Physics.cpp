#include "Physics.h"
#include <include/havok.h>

#include <Component/AABB.h>
#include <Component/Transform/Transform.h>
#include <Core/GameObject.h>

#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/PhysicsManager.h>
#include <Manager/HavokCore.h>

Physics::Physics(GameObject *parent) :
	parent(parent)
{
}

Physics::Physics(Physics &physics)
{

}

Physics::~Physics() {
	RemoveFromWorld();
}

void Physics::LoadHavokFile(const string &fileName)
{
	body = Manager::Physics->GetFirstRigidBody(fileName.c_str());
}

void Physics::Update()
{
	// Test if was removed from world and skip updating
	if (body->getReferenceCount() == 0) {
		Manager::Scene->RemoveObject(this->parent);
		return;
	}

	if (body->isActive())
	{
		float val[4];
		hkVector4 pos = body->getPosition();
		memcpy(val, &pos, 4 * sizeof(float));
		parent->transform->SetWorldPosition(glm::vec3(val[0], val[1], val[2]));

		hkQuaternionf qrot = body->getRotation();
		memcpy(val, &qrot.m_vec, 4 * sizeof(float));
		parent->transform->SetWorldRotation(glm::quat(val[3], val[0], val[1], val[2]));
	}
}

void Physics::AddToWorld()
{
	glm::vec3 pos = parent->transform->GetWorldPosition();
	glm::quat rot = parent->transform->GetWorldRotation();
	hkVector4 hkpos(pos.x, pos.y, pos.z, 0);
	hkQuaternion hkrot(rot.x, rot.y, rot.z, rot.w);
	body->setPositionAndRotation(hkpos, hkrot);
	Manager::Havok->AddIntoTheWorld(body);
}

void Physics::RemoveFromWorld() 
{
	if (body->isAddedToWorld()) {
		Manager::Havok->RemoveFromTheWorld(this->body);
	}
}


void Physics::Deactivate()
{
}

void Physics::UpdatePhysicsEngine()
{
	Manager::Havok->MarkForWrite();

	auto pos = parent->transform->GetWorldPosition();
	auto rot = parent->transform->GetWorldRotation();

	body->setPositionAndRotation(hkVector4(pos.x, pos.y, pos.z, 0), hkQuaternion(rot.x, rot.y, rot.z, rot.w));
	body->setLinearVelocity(hkVector4());

	Manager::Havok->UnmarkForWrite();
}

void Physics::SetLinearVelocity(glm::vec3 velocity)
{
	Manager::Havok->MarkForWrite();
	body->setLinearVelocity(hkVector4(velocity.x, velocity.y, velocity.z));
	Manager::Havok->UnmarkForWrite();
}
