#include "pch.h"
#include "PhysicsBall.h"

#include <PhysX/PxPhysicsAPI.h>
#include <Physics/PhysXManager.h>

PhysicsBall::PhysicsBall(float radius, glm::vec3 pos, glm::vec3 direction)
	: GameObject("")
{
	SetName("ball");
	SetMesh(Manager::GetResource()->GetMesh("sphere"));

	transform->SetScale(glm::vec3(2 * radius));

	auto pxManager = Singleton<PhysXManager>::Instance();

	PxVec3 origin(pos.x, pos.y, pos.z);

	direction *= ((rand() % 10) + 10);
	PxVec3 pxVelocity(direction.x, direction.y, direction.z);

	PxMaterial *material = pxManager->gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	pxBody = PxCreateDynamic(*pxManager->gPhysics, PxTransform(origin), PxSphereGeometry(radius), *material, 10.0f);
	pxBody->setAngularDamping(0.5f);
	pxBody->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	pxBody->setLinearVelocity(pxVelocity);
	pxManager->gScene->addActor(*pxBody);

	timer = Manager::GetEvent()->standardTimers->Create(EventType::TIMER_EXPIRED, 5, this);
	Manager::GetEvent()->standardTimers->Add(*timer);
	SubscribeToEvent(EventType::TIMER_EXPIRED);
}

PhysicsBall::~PhysicsBall()
{
	UnsubscribeFrom(EventType::TIMER_EXPIRED);
	auto pxManager = Singleton<PhysXManager>::Instance();
	pxManager->gScene->removeActor(*pxBody);
}

void PhysicsBall::Update()
{
	GameObject::Update();

	auto pxManager = Singleton<PhysXManager>::Instance();
	auto pxT = pxBody->getGlobalPose();
	auto pxPos = pxT.p;
	auto pxRot = pxT.q;
	transform->SetWorldPosition(glm::vec3(pxPos.x, pxPos.y, pxPos.z));
	transform->SetWorldRotation(glm::quat(pxRot.w, pxRot.x, pxRot.y, pxRot.z));
}

void PhysicsBall::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::TIMER_EXPIRED && data == this) {
		Manager::GetScene()->RemoveObject(this, true);
		Manager::GetEvent()->standardTimers->Remove(*timer);
	}
}
