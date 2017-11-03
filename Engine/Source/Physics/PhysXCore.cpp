#include "PhysXCore.h"

#ifdef PHYSICS_ENGINE

#include <iostream>
#include <include/glm.h>
#include <Core/GameObject.h>

#include <Component/Physics.h>
#include <Component/GameScript.h>

using namespace std;
using namespace physx;

// Energy-Surface
//#define PVD_HOST	"192.168.0.101"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.
#define PVD_HOST		"127.0.0.1"	
#define PVD_PORT		5425
#define PVD_TIMEOUT		10

class PhsyXEventCallback : public physx::PxSimulationEventCallback
{
	public:
		PhsyXEventCallback(PhysXCore *phsyX);

		void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override {};
		void onWake(physx::PxActor** actors, physx::PxU32 count) override {};
		void onSleep(physx::PxActor** actors, physx::PxU32 count) override {};
		void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override {};
		void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;

	private:
		PhysXCore *phsyX;
};

PxMaterial *gMaterial = NULL;
PxReal stackZ = 10.0f;

void createStack(PxPhysics* gPhysics, PxScene* gScene, const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	for (PxU32 i = 0; i<size; i++)
	{
		for (PxU32 j = 0; j<size - i; j++)
		{
			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			gScene->addActor(*body);
		}
	}
	shape->release();
}

// spherical joint limited to an angle of at most pi/4 radians (45 degrees)
PxJoint* createLimitedSpherical(PxPhysics* gPhysics, PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxSphericalJoint* j = PxSphericalJointCreate(*gPhysics, a0, t0, a1, t1);
	j->setLimitCone(PxJointLimitCone(PxPi / 4, PxPi / 4, 0.05f));
	j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
	return j;
}

// revolute joint limited to an angle of at most pi/4 radians (45 degrees)

// fixed, breakable joint
PxJoint* createBreakableFixed(PxPhysics* gPhysics, PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxFixedJoint* j = PxFixedJointCreate(*gPhysics, a0, t0, a1, t1);
	j->setBreakForce(1000, 100000);
	j->setConstraintFlag(PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, true);
	return j;
}

// D6 joint with a spring maintaining its position
PxJoint* createDampedD6(PxPhysics* gPhysics, PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxD6Joint* j = PxD6JointCreate(*gPhysics, a0, t0, a1, t1);
	j->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLOCKED);
	j->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
	j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLOCKED);
	j->setDrive(PxD6Drive::eSLERP, PxD6JointDrive(0, 1000, FLT_MAX, true));
	return j;
}

typedef PxJoint* (*JointCreateFunction)(PxPhysics* gPhysics, PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1);


void createChain(PxPhysics* gPhysics, PxScene* gScene, const PxTransform& t, PxU32 length, const PxGeometry& g, PxReal separation, JointCreateFunction createJoint)
{
	PxVec3 offset(separation / 2, 0, 0);
	PxTransform localTm(offset);
	PxRigidDynamic* prev = NULL;

	for (PxU32 i = 0;i<length;i++)
	{
		PxRigidDynamic* current = PxCreateDynamic(*gPhysics, t*localTm, g, *gMaterial, 1.0f);
		(*createJoint)(gPhysics, prev, prev ? PxTransform(offset) : t, current, PxTransform(-offset));
		gScene->addActor(*current);
		prev = current;
		localTm.p.x += separation;
	}
}

PhysXCore::PhysXCore()
{
	printf("[PhysX version] %d.%d.%d", PX_PHYSICS_VERSION_MAJOR, PX_PHYSICS_VERSION_MINOR, PX_PHYSICS_VERSION_BUGFIX);

	gPhysics	= nullptr;
	gCooking	= nullptr;
	gFoundation	= nullptr;
	gDispatcher	= nullptr;
	gScene		= nullptr;
}

static PxFilterFlags filterShader(	PxFilterObjectAttributes attributes0,
									PxFilterData filterData0,
									PxFilterObjectAttributes attributes1,
									PxFilterData filterData1,
									PxPairFlags& pairFlags,
									const void* constantBlock,
									PxU32 constantBlockSize)
{
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	pairFlags |= PxPairFlag::eDETECT_DISCRETE_CONTACT;

	if (!PxFilterObjectIsKinematic(attributes0) && !PxFilterObjectIsKinematic(attributes1))
	{
		pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
	}

	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	return PxFilterFlag::eDEFAULT;
}

PhysXCore::~PhysXCore() {
}

void PhysXCore::Init()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(gFoundation);

	PxTolerancesScale scale;
	scale.length = 1;		// typical length of an object
	scale.speed = 9.81f;	// typical speed of an object, gravity * 1s is a reasonable choice

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, scale, true, profileZoneManager);
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(scale));

	if (gPhysics->getPvdConnectionManager())
	{
		gPhysics->getVisualDebugger()->setVisualizeConstraints(true);
		gPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
		gPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
		gConnection = PxVisualDebuggerExt::createConnection(gPhysics->getPvdConnectionManager(), PVD_HOST, PVD_PORT, PVD_TIMEOUT);
	}

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = filterShader;
	sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
	
	gScene = gPhysics->createScene(sceneDesc);
	gScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
	auto triggerEventCallback = new PhsyXEventCallback(this);
	gScene->setSimulationEventCallback(triggerEventCallback);

	CreateScene();
}

void PhysXCore::StepPhysics(float deltaTime)
{
	gScene->simulate(deltaTime);
}

void PhysXCore::FetchResults() const
{
	gScene->fetchResults(true);
}

void PhysXCore::CleanupPhysics()
{
	gScene->release();
	gDispatcher->release();
	PxProfileZoneManager* profileZoneManager = gPhysics->getProfileZoneManager();
	if (gConnection != NULL)
		gConnection->release();
	gPhysics->release();
	profileZoneManager->release();
	gFoundation->release();
}

void PhysXCore::CreateScene()
{
	// Scene
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	gScene->addActor(*groundPlane);

	//for (PxU32 i = 0; i < 1; i++)
	//	createStack(gPhysics, gScene, PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);

	//createChain(gPhysics, gScene, PxTransform(PxVec3(0.0f, 20.0f, 0.0f)), 5, PxBoxGeometry(2.0f, 0.5f, 0.5f), 4.0f, createLimitedSpherical);
	//createChain(gPhysics, gScene, PxTransform(PxVec3(0.0f, 20.0f, -10.0f)), 5, PxBoxGeometry(2.0f, 0.5f, 0.5f), 4.0f, createBreakableFixed);
	//createChain(gPhysics, gScene, PxTransform(PxVec3(0.0f, 40.0f, 20.0f)), 5, PxBoxGeometry(2.0f, 0.5f, 0.5f), 6.0f, createDampedD6);
}

physx::PxDefaultAllocator* PhysXCore::GetPxAlocator()
{
	return &gAllocator;
}

physx::PxCooking * PhysXCore::GetPxCooking() const
{
	return gCooking;
}

physx::PxPhysics * PhysXCore::GetPhysics() const
{
	return gPhysics;
}

physx::PxScene * PhysXCore::GetPxScene() const
{
	return gScene;
}

void PhysXCore::AddActorToScene(physx::PxActor &actor)
{
	gScene->addActor(actor);
}

void PhysXCore::AddCollection(physx::PxCollection & collection)
{
	gScene->addCollection(collection);
}

void PhysXCore::RemoveActorFromScene(physx::PxActor & actor)
{
	gScene->removeActor(actor);
}

/**
	Callback event from PxScene
*/

PhsyXEventCallback::PhsyXEventCallback(PhysXCore * phsyX)
	: phsyX(phsyX)
{
}

void PhsyXEventCallback::onTrigger(physx::PxTriggerPair * pairs, physx::PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		auto other = static_cast<GameObject*>(pairs[i].otherActor->userData);
		auto trigger = static_cast<GameObject*>(pairs[i].triggerActor->userData);

		if (pairs[i].status == PxPairFlag::Enum::eNOTIFY_TOUCH_FOUND) {
			trigger->TriggerEnter(other);
		}
		else {
			trigger->TriggerExit(other);
		}
	}
}

#endif // end PHYSICS_ENGINE

