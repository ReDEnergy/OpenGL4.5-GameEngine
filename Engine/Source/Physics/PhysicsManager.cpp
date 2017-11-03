#include "PhysicsManager.h"

#ifdef PHYSICS_ENGINE

#include <templates/singleton.h>

#include <Core/GameObject.h>

#include <Manager/Manager.h>
#include <Manager/DebugInfo.h>

#include <Physics/PhysXCore.h>


using namespace std;

// TODO Share Shapes

PhysicsManager::PhysicsManager()
{
	Manager::Debug->InitManager("Physics");
	phsyXCore = SINGLETON(PhysXCore);
}

PhysicsManager::~PhysicsManager()
{
}

PhysXCore * PhysicsManager::GetPhysX() const
{
	return phsyXCore;
}

void PhysicsManager::AddObject(GameObject * obj)
{
	//phsyXCore->AddActorToScene(obj->Get)
}

void PhysicsManager::RemoveObject(GameObject * obj)
{
}

#endif