#include "Physics.h"

#ifdef PHYSICS_ENGINE

#include <iostream>

#include <Component/AABB.h>
#include <Component/Transform/Transform.h>
#include <Core/GameObject.h>

#include <Manager/Manager.h>
#include <Manager/SceneManager.h>

#include <UI/ColorPicking/ColorPicking.h>

#include <PxPhysicsAPI.h>
#include <Physics/PhysXCore.h>
#include <Physics/PhysicsManager.h>

using namespace std;
using namespace physx;

static PxMaterial *dynamicMaterial = nullptr;
static PxMaterial *staticMaterial = nullptr;

static_assert(sizeof(PxQuat) == sizeof(glm::quat), "Error! PxQuat not compatible with glm::quat");
static_assert(sizeof(PxVec4) == sizeof(glm::vec4), "Error! PxVec3 not compatible with glm::vec4");
static_assert(sizeof(PxVec3) == sizeof(glm::vec3), "Error! PxVec3 not compatible with glm::vec3");
static_assert(sizeof(PxVec2) == sizeof(glm::vec2), "Error! PxVec3 not compatible with glm::vec2");

Physics::Physics(GameObject *object) : GO(object)
{
	Init();
}

Physics::Physics(Physics &physics)
{
	Init();
}

Physics::~Physics()
{
}

void Physics::Init()
{
	isDynamic = false;
	isTrigger = false;
	isKinematic = false;

	applyGravity = false;

	actor = nullptr;
	shape = nullptr;
	staticBody = nullptr;
	dynamicBody = nullptr;
}

void Physics::SetGameObject(GameObject *object)
{
	GO = object;
}

bool Physics::IsActive() const
{
	return (actor != nullptr);
}

void Physics::SetRigidBody()
{
	if (!GO) return;

	if (actor) {
		UpdateScale();
		return;
	}

	auto PX = Manager::GetPhysics()->GetPhysX();
	auto PxCore = PX->GetPhysics();
	auto halfExtend = GO->transform->GetScale() / 2.0f;
	if (halfExtend == glm::vec3(0)) {
		return;
	}

	if (staticMaterial == nullptr) {
		staticMaterial = PxCore->createMaterial(1.0f, 1.0f, 0.5f);
		dynamicMaterial = PxCore->createMaterial(0.5f, 0.5f, 0.3f);
	}

	shape = PxCore->createShape(PxBoxGeometry(halfExtend.x, halfExtend.y, halfExtend.z), *dynamicMaterial);

	if (shape) {
		isDynamic = true;

		auto pos = GO->transform->GetWorldPosition();
		auto rot = GO->transform->GetWorldRotation();
		PxTransform tr(PxVec3(pos.x, pos.y, pos.z));
		tr.q = PxQuat(rot.x, rot.y, rot.z, rot.w);

		staticBody = PxCore->createRigidStatic(tr);
		dynamicBody = PxCore->createRigidDynamic(tr);

		staticBody->userData = GO;
		dynamicBody->userData = GO;

		if (isDynamic) {
			actor = dynamicBody;
			applyGravity = true;
		}
		else {
			actor = staticBody;
		}

		actor->attachShape(*shape);
		PX->AddActorToScene(*actor);

		if (dynamicBody) {
			//float mass = 8000 * halfExtend.x * halfExtend.y * halfExtend.z;
			PxRigidBodyExt::updateMassAndInertia(*dynamicBody, 100);
			//PxRigidBodyExt::setMassAndUpdateInertia(*dynamicBody, mass);
		}
	}
}

void Physics::SetIsDynamic(bool state)
{
	if (state == isDynamic) return;
	isDynamic = state;

	auto PX = Manager::GetPhysics()->GetPhysX();
	auto inScene = actor->getScene();

	auto TR = actor->getGlobalPose();
	actor->detachShape(*shape);
	PX->RemoveActorFromScene(*actor);

	if (isDynamic) {
		actor = dynamicBody;
		isKinematic ? dynamicBody->setKinematicTarget(TR) : dynamicBody->setGlobalPose(TR);
	}
	else {
		actor = staticBody;
		actor->setGlobalPose(TR);
	}

	actor->attachShape(*shape);
	PX->AddActorToScene(*actor);
}

void Physics::SetGravity(bool state)
{
	if (!isDynamic) return;

	applyGravity = !state;
	actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !applyGravity);
	dynamicBody->wakeUp();
}

void Physics::SetMass(float mass)
{
	if (!actor || !isDynamic) return;
	PxRigidBodyExt::setMassAndUpdateInertia(*dynamicBody, mass);
}

void Physics::SetIsKinematic(bool state)
{
	if (!actor || !isDynamic) return;

	isKinematic = state;
	dynamicBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, state);
	if (dynamicBody && state == false) {
		dynamicBody->wakeUp();
	}
}

void Physics::SetIsTrigger(bool trigger)
{
	if (!actor || isTrigger == trigger)
	{
		return;
	}

	isTrigger = trigger;

	actor->detachShape(*shape);
	if (trigger == true)
	{
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
	else {
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	}
	actor->attachShape(*shape);
}

void Physics::SetSimulationState(bool state)
{
	actor->detachShape(*shape);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, state);
	actor->attachShape(*shape);
	if (state == true) {
		SetTransform();
		if (isDynamic)
			dynamicBody->wakeUp();
	}
}

void Physics::SetTransform()
{
	if (actor && GO->transform->GetMotionState()) {
		auto pos = GO->transform->GetWorldPosition();
		auto rot = GO->transform->GetWorldRotation();

		PxTransform tr(PxVec3(pos.x, pos.y, pos.z));
		tr.q = PxQuat(rot.x, rot.y, rot.z, rot.w);

		if (dynamicBody && isKinematic) {
			dynamicBody->setKinematicTarget(tr);
		}
		else {
			actor->setGlobalPose(tr);
		}
	}
}

void Physics::ReadTransform()
{
	PxTransform tr = actor->getGlobalPose();
	GO->transform->SetWorldRotation(glm::quat(tr.q.w, tr.q.x, tr.q.y, tr.q.z));
	GO->transform->SetWorldPosition(glm::vec3(tr.p.x, tr.p.y, tr.p.z));
}


void Physics::UpdateScale()
{
	auto halfExtend = GO->transform->GetScale() / 2.0f;
	actor->detachShape(*shape);
	shape->setGeometry(PxBoxGeometry(halfExtend.x, halfExtend.y, halfExtend.z));
	actor->attachShape(*shape);
}

bool Physics::HasGravity() const
{
	return applyGravity;
}

bool Physics::IsDynamic() const
{
	return isDynamic;
}

bool Physics::IsKinematic() const
{
	return isKinematic;
}

bool Physics::IsTrigger() const
{
	return isTrigger;
}

void Physics::Update()
{
	if (!actor)
	{
		return;
	}

	if ((actor == staticBody) || (actor == dynamicBody && isKinematic)) {
		SetTransform();
	}
	else if (!dynamicBody->isSleeping()) {
		ReadTransform();
	}
}

#endif // PHYSICS_ENGINE
