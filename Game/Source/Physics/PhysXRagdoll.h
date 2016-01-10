#pragma once
#include "pch.h"

#include <string>
#include <unordered_map>

#include <PhysX/PxPhysicsAPI.h>

using namespace physx;
using namespace std;

class PhysXManager;

class PhysXRagdoll : public ObjectInput
{
	public:
		PhysXRagdoll(PhysXManager * const pxManager);
		~PhysXRagdoll();

		void Load(const string &fileName);
		void Update(float deltaTime, Camera * camera);
		void Update(float deltaTime, int mods);
		void OnKeyPress(int key, int mods);
		void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods);

	private:
		Transform *transform;
		PhysXManager * const pxManager;
		unsigned int dynamicSphere;

	public:
		unordered_map <string, PxActor*> actors;
		unordered_map <string, PxD6Joint*> joints;
};