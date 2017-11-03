#include "PhysXRagdoll.h"

#ifdef PHYSICS_ENGINE

#include <fstream>
#include <iostream>
#include <vector>

#include <Core/Camera/Camera.h>
#include <Core/WindowObject.h>
#include <Component/Transform/Transform.h>

#include <include/gl.h>
#include <include/glm.h>

#include <Physics/PhysXCore.h>

using namespace std;
using namespace physx;

char *jointID = new char[100];
unsigned char nextJointID = 0;
unordered_map<string, PxD6Joint*> jointTests;
vector<PxRigidDynamic*> jointBlocks;

// D6 joint with a spring maintaining its position
PxJoint* createDampedD62(PxPhysics* gPhysics, PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	string name("jointTest");
	PxD6Joint* j = PxD6JointCreate(*gPhysics, a0, t0, a1, t1);
	j->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
	j->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
	j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLOCKED);
	//j->setDrive(PxD6Drive::eSLERP, PxD6JointDrive(0, 1000, FLT_MAX, true));
	j->setDrive(PxD6Drive::eSWING, PxD6JointDrive(0, 1000, FLT_MAX, true));
	j->setProjectionLinearTolerance(0.1f);
	//j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);

	sprintf(jointID, "jointTest%d", nextJointID++);
	jointTests[jointID] = j;

	return j;
}

typedef PxJoint* (*JointCreateFunction)(PxPhysics* gPhysics, PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1);

void createChain2(PxPhysics* gPhysics, PxScene* gScene, const PxTransform& t, PxU32 length, const PxGeometry& g, PxReal separation, JointCreateFunction createJoint)
{
	PxVec3 offset(separation / 2, 0, 0);
	PxTransform localTm(offset);
	PxRigidDynamic* prev = NULL;
	PxMaterial *gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	for (PxU32 i = 0;i<length;i++)
	{
		PxRigidDynamic* current = PxCreateDynamic(*gPhysics, t*localTm, g, *gMaterial, 10.0f);
		//current->setSolverIterationCounts(8, 2);
		if (prev == NULL) 
			current->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		current->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		(*createJoint)(gPhysics, prev, prev ? PxTransform(offset) : t, current, PxTransform(-offset));
		gScene->addActor(*current);
		prev = current;
		localTm.p.x += separation;
		jointBlocks.push_back(current);
	}
}

PxRigidDynamic* createDynamic(PxPhysics* gPhysics, PxScene* gScene, const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0))
{
	PxMaterial *material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *material, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}

PhysXRagdoll::PhysXRagdoll(PhysXCore * const pxManager)
	: pxManager(pxManager)
{
	transform = new Transform();
	dynamicSphere = 0;
}

PhysXRagdoll::~PhysXRagdoll() {
}

void PhysXRagdoll::Load(const string &fileName)
{
	ifstream pxFile(fileName.c_str(), ios::in);
	streampos begin = pxFile.tellg();
	pxFile.seekg(0, ios::end);
	streampos end = pxFile.tellg();
	auto fileSize = end - begin;
	
	PxU8* data = new PxU8[fileSize];
	pxFile.seekg(0, ios::beg);
	pxFile.read((char*)data, fileSize);

	//PxAllocatorCallback;
	PxStringTable *stringTable = &PxStringTableExt::createStringTable(*pxManager->GetPxAlocator());
	PxSerializationRegistry* registry = PxSerialization::createSerializationRegistry(*pxManager->GetPhysics());

	PxDefaultMemoryInputData sharedInputStream(data, static_cast<PxU32>(fileSize));
	PxCollection* collection = PxSerialization::createCollectionFromXml(sharedInputStream, *pxManager->GetPxCooking(), *registry, 0, stringTable);

	PxU32 nrObject = collection->getNbObjects();
	for (PxU32 i = 0; i < collection->getNbObjects(); i++)
	{

		PxType type = collection->getObject(i).getConcreteType();
		const char* typeName = collection->getObject(i).getConcreteTypeName();
		cout << "type: " << typeName << " [" << type <<"]"<< "=> ";
		if (type == PxConcreteType::eRIGID_DYNAMIC)
		{
			PxActor* actor = collection->getObject(i).is<PxActor>();
			if (actor) {
				const char *name = actor->getName();
				if (name) {
					cout << name << endl;
					actors[name] = actor;
				}

				if (strcmp(name, "Hips") == 0)
					continue;

				// Disable Gravity for Skeleton
				actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

				// Make Skeleton Dynamic
				PxRigidBody *body = actor->isRigidBody();
				if (body) {
					body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
				}
			}
		}
		if (type == PxConcreteType::eCONSTRAINT) {
			PxRigidActor *actor0, *actor1;
			PxConstraint* constraint = collection->getObject(i).is<PxConstraint>();
			constraint->getActors(actor0, actor1);
			const char* name0 = actor0->getName();
			const char* name1 = actor1->getName();
			cout << "Constraint: " << name0 << " -> " << name1 << endl;
		}

		if (type == (PxConcreteType::eFIRST_PHYSX_EXTENSION + PxConcreteType::eRIGID_DYNAMIC)) {
			PxD6Joint *joint = collection->getObject(i).is<PxD6Joint>();
			joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
			joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);
			joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLOCKED);
			const char *name = joint->getName();
			PxScene *scene = joint->getScene();
			cout << "Joint: " << name << endl;
			joints[name] = joint;
		}
		
	}

	pxManager->AddCollection(*collection);

	createChain2(pxManager->GetPhysics(), pxManager->GetPxScene(), PxTransform(PxVec3(0.0f, 40.0f, 20.0f)), 5, PxBoxGeometry(2.0f, 0.5f, 0.5f), 6.0f, createDampedD62);

	cout << actors.size() << endl;
	collection->release();
}

void PhysXRagdoll::Update(float deltaTime, Camera *camera)
{
	if (dynamicSphere) {
		dynamicSphere--;
		glm::vec3 pos = camera->transform->GetWorldPosition();
		glm::vec3 forward = camera->transform->GetLocalOZVector();
		PxVec3 origin(pos.x, pos.y, pos.z);
		PxVec3 velocity(forward.x, forward.y, forward.z);
		createDynamic(pxManager->GetPhysics(), pxManager->GetPxScene(), PxTransform(origin), PxSphereGeometry(10), 200 * velocity);
	}
}

void PhysXRagdoll::Update(float deltaTime, int mods)
{
	if (mods != GLFW_MOD_SHIFT) return;

	transform->Move(glm::vec3(100, 0, 0), deltaTime);

	if (window->KeyHold(GLFW_KEY_L)) {
		PxD6Joint *joint = joints["RightUpLegConstraint1"];
		if (joint) {
			PxReal sy = joint->getSwingYAngle();
			PxReal sz = joint->getSwingZAngle();
			PxReal tw = joint->getTwist();
			cout << "Rot: " << sy << ", " << sz << ", " << tw << endl;
		}
	}

	if (window->KeyHold(GLFW_KEY_I) && mods == GLFW_MOD_SHIFT) {
		PxD6Joint *joint = jointTests["jointTest1"];
		if (joint) {
			PxReal sy = joint->getSwingYAngle();
			PxReal sz = joint->getSwingZAngle();
			PxReal tw = joint->getTwist();
			cout << "Rot: " << sy << ", " << sz << ", " << tw << endl;
		}
	}
	bool ROTATE = false;
	if (window->KeyHold(GLFW_KEY_UP)) {
		transform->RotateWorldOX(deltaTime);
		ROTATE = true;
	}
	if (window->KeyHold(GLFW_KEY_DOWN)) {
		transform->RotateWorldOX(-deltaTime);
		ROTATE = true;
	}
	if (window->KeyHold(GLFW_KEY_LEFT)) {
		transform->RotateWorldOY(-deltaTime);
		ROTATE = true;
	}
	if (window->KeyHold(GLFW_KEY_RIGHT)) {
		transform->RotateWorldOY(deltaTime);
		ROTATE = true;
	}
	if (ROTATE) {
		PxRigidActor *actor0, *actor1;
		PxD6Joint *joint = jointTests["jointTest0"];
		if (joint) {
			joint->getActors(actor0, actor1);
			PxTransform pose = actor1->getGlobalPose();
			PxRigidDynamic *act = actor1->isRigidDynamic();
			if (act) {
				glm::quat rot = transform->GetWorldRotation();
				pose.q = PxQuat(rot.x, rot.y, rot.z, rot.w);
				act->setKinematicTarget(pose);
			}
		}
	}
}

bool KinematicBody = false;

void PhysXRagdoll::OnKeyPress(int key, int mods)
{
	if (mods != GLFW_MOD_SHIFT)
		return;

	if (key == GLFW_KEY_H) {
		PxD6Joint *joint = jointTests["jointTest3"];
		PxRigidActor *actor0, *actor1;
		joint->getActors(actor0, actor1);
		PxRigidBody* body = actor0->isRigidBody();
		if (body) {
			body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, KinematicBody);
			KinematicBody = !KinematicBody;
		}
	}
}

void PhysXRagdoll::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	if (button == 1)
		dynamicSphere++;
}

#endif
