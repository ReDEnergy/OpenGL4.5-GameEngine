//#include <pch.h>
#include "Physics.h"
#include <include/havok.h>

#include <Component/AABB.h>
#include <Component/Transform.h>
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

void Physics::LoadHavokFile(const char *fileName) {
	body = Manager::Physics->GetFirstRigidBody(fileName);
}

void Physics::Update() {
	// Test if was removed from world and skip updating
	if (body->getReferenceCount() == 0) {
		Manager::Scene->RemoveObject(this->parent);
		return;
	}
	if (body->isActive()) {
		float val[4];
		hkVector4 pos = body->getPosition();
		memcpy(val, &pos, 4 * sizeof(float));
		parent->transform->position = glm::vec3(val[0], val[1], val[2]);

		hkQuaternionf qrot = body->getRotation();
		memcpy(val, &qrot.m_vec, 4 * sizeof(float));
		parent->transform->rotationQ = glm::quat(val[3], val[0], val[1], val[2]);

		parent->transform->Update();
	}
}

void Physics::AddToWorld() {
	Transform *T = parent->transform;
	hkVector4 pos(T->position.x, T->position.y, T->position.z, 0);
	hkQuaternion rot(T->rotationQ.x, T->rotationQ.y, T->rotationQ.z, T->rotationQ.w);
	body->setPositionAndRotation(pos, rot);
	Manager::Havok->AddIntoTheWorld(body);
}

void Physics::RemoveFromWorld() {
	if (body->isAddedToWorld()) {
		Manager::Havok->RemoveFromTheWorld(this->body);
	}
}


void Physics::Deactivate() {
}

void Physics::SetPosition(glm::vec3 pos) {
	Manager::Havok->MarkForWrite();
	body->setPosition(hkVector4(pos.x, pos.y, pos.z, 0));
	Manager::Havok->UnmarkForWrite();

	parent->transform->position = pos;
	parent->transform->Update();
}