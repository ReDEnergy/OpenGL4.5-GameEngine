#include "pch.h"
#include "KinematicBall.h"

#include <PhysX/PxPhysicsAPI.h>
#include <Physics/PhysXManager.h>

KinematicBall::KinematicBall(float radius)
	: GameObject("")
{
	SetName("ball");
	SetMesh(Manager::GetResource()->GetMesh("sphere"));

	auto pxManager = Singleton<PhysXManager>::Instance();

	auto pos = glm::vec3(float(rand() % 10), 10, float(rand() % 10));
	transform->SetWorldPosition(pos);
	transform->SetScale(glm::vec3(2 * radius));

	PxVec3 origin(pos.x, pos.y, pos.z);
	PxMaterial *material = pxManager->gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	pxBody = PxCreateDynamic(*pxManager->gPhysics, PxTransform(origin), PxSphereGeometry(radius), *material, 10.0f);
	pxBody->setAngularDamping(0.5f);
	pxBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	pxManager->gScene->addActor(*pxBody);
}

KinematicBall::~KinematicBall()
{
}

void KinematicBall::SetPosition(glm::vec3 pos)
{
	PxTransform t(PxVec3(pos.x, pos.y, pos.z));
	pxBody->setKinematicTarget(t);
}

void KinematicBall::SetPositionAndRotation(glm::vec3 pos, glm::quat rot)
{
	PxTransform t(PxVec3(pos.x, pos.y, pos.z), PxQuat(rot.x, rot.y, rot.z, rot.w));
	pxBody->setKinematicTarget(t);
}

void KinematicBall::Update()
{
	GameObject::Update();

	if (!pxBody->isSleeping() || transform->GetMotionState())
	{
		auto pxManager = Singleton<PhysXManager>::Instance();
		auto pxT = pxBody->getGlobalPose();
		auto pxPos = pxT.p;
		auto pxRot = pxT.q;
		transform->SetWorldPosition(glm::vec3(pxPos.x, pxPos.y, pxPos.z));
		transform->SetWorldRotation(glm::quat(pxRot.w, pxRot.x, pxRot.y, pxRot.z));
	}
}
