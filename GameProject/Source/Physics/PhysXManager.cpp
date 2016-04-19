#include <pch.h>
#include "PhysXManager.h"

using namespace std;
using namespace physx;

// Energy-Surface
//#define PVD_HOST	"192.168.0.101"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.
#define PVD_HOST	"127.0.0.1"	
#define PVD_PORT	5425
#define PVD_TIMEOUT	10

PxMaterial *gMaterial = NULL;
PxReal stackZ = 10.0f;

void createStack(PxPhysics* gPhysics, PxScene* gScene, const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	for (PxU32 i = 0; i<size;i++)
	{
		for (PxU32 j = 0;j<size - i;j++)
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

PhysXManager::PhysXManager() {
	gPhysics	= NULL;
	gCooking	= NULL;
	gFoundation	= NULL;
	gDispatcher	= NULL;
	gScene		= NULL;
}

PhysXManager::~PhysXManager() {
}

static PxFilterFlags filterShader(
	PxFilterObjectAttributes attributes0,
	PxFilterData filterData0,
	PxFilterObjectAttributes attributes1,
	PxFilterData filterData1,
	PxPairFlags& pairFlags,
	const void* constantBlock,
	PxU32 constantBlockSize)
{
	pairFlags = PxPairFlag::eSOLVE_CONTACT;
	pairFlags |= PxPairFlag::eDETECT_DISCRETE_CONTACT;
	pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
	return PxFilterFlags();
}

void PhysXManager::Init()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(gFoundation);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, profileZoneManager);
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));

	if (gPhysics->getPvdConnectionManager())
	{
		gPhysics->getVisualDebugger()->setVisualizeConstraints(true);
		gPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
		gPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
		gConnection = PxVisualDebuggerExt::createConnection(gPhysics->getPvdConnectionManager(), PVD_HOST, PVD_PORT, PVD_TIMEOUT);
	}

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = filterShader;
	sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
	
	gScene = gPhysics->createScene(sceneDesc);

	gScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);

	CreateScene();

}

void PhysXManager::StepPhysics(float deltaTime)
{
	gScene->simulate(deltaTime);
	gScene->fetchResults(true);
}

void PhysXManager::CleanupPhysics()
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

void PhysXManager::CreateScene()
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
