#pragma once
#include <string>
#include <unordered_map>

#include <PhysX/PxPhysicsAPI.h>
#include <Component/ObjectInput.h>

using namespace physx;

class Camera;
class Transform;
class PhysXManager;

class PhysXRagdoll : public ObjectInput
{
	public:
		PhysXRagdoll(PhysXManager * const pxManager);
		~PhysXRagdoll();

		void Load(const std::string &fileName);
		void Update(float deltaTime, Camera * camera);
		void Update(float deltaTime, int mods);
		void OnKeyPress(int key, int mods);
		void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods);

	private:
		Transform *transform;
		PhysXManager * const pxManager;
		unsigned int dynamicSphere;

	public:
		std::unordered_map<std::string, PxActor*> actors;
		std::unordered_map<std::string, PxD6Joint*> joints;
};