#include "PhysicsManager.h"

#include <Manager/Manager.h>
#include <Manager/HavokCore.h>
#include <Manager/DebugInfo.h>
#include <cstdio>

// Math and base includes
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>

// Loading Havok Files
#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Common/Serialize/Util/hkStructureLayout.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Physics2012/Utilities/Serialize/hkpPhysicsData.h>

// Shapes
#include <Physics2012/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics2012/Dynamics/Entity/hkpRigidBodyCinfo.h>

using namespace std;

// TODO Share Shapes

PhysicsManager::PhysicsManager() {
	Manager::Debug->InitManager("Physics");
}

PhysicsManager::~PhysicsManager() {
}

hkpRigidBody* PhysicsManager::GetBox(float X, float Y, float Z) {
	hkVector4 halfExtent( X, Y, Z);
	hkpBoxShape* shape = new hkpBoxShape(halfExtent, 0.0f);

	hkpRigidBodyCinfo ci;
	ci.m_shape = shape;

	hkpRigidBody *body = new hkpRigidBody(ci);
	return body;
}

hkpRigidBody* PhysicsManager::GetCopyOf(const hkpRigidBody *body) {
	hkpRigidBodyCinfo ci;
	Manager::Havok->MarkForRead();
		body->getCinfo(ci);
	Manager::Havok->UnmarkForRead();
	hkpRigidBody *rb = new hkpRigidBody(ci);
	return rb;
}

hkpRigidBody* PhysicsManager::GetFirstRigidBody(const char* const fileName) {
	hkSerializeUtil::ErrorDetails loadError;

	hkStringBuf obj(fileName);
	hkResource* LoadedData;
	LoadedData = hkSerializeUtil::load(obj.cString(), &loadError);
	HK_ASSERT3(0xa6451543, LoadedData != HK_NULL, "Could not load file. The error is:\n" << loadError.defaultMessage.cString() );

	printf("Physics file: %s\n", fileName);

	// Get the top level object in the file, which we know is a hkRootLevelContainer
	hkRootLevelContainer* container = LoadedData->getContents<hkRootLevelContainer>();
	HK_ASSERT2(0xa6451543, container != HK_NULL, "Could not load root level object" );

	hkpPhysicsData* physicsData = static_cast<hkpPhysicsData*>( container->findObjectByType( hkpPhysicsDataClass.getName() ) );
	HK_ASSERT2(0x0, physicsData != HK_NULL, "Could not find physics data in root level object" );

	hkpPhysicsSystem* pSystem = physicsData->getPhysicsSystems()[0];
	hkpPhysicsSystem* physicsSystem = pSystem->clone();
	const hkArray<hkpRigidBody *>& bodies = physicsSystem->getRigidBodies();

	return bodies[0];
}