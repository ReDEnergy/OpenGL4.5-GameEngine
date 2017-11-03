#pragma once
#ifdef PHYSICS_ENGINE

#include <string>
#include <unordered_map>

#include <Component/ObjectInput.h>

class Camera;
class Transform;

// PhysX
namespace physx {
	class PxActor;
	class PxD6Joint;
}
class PhysXCore;

class PhysXRagdoll : public ObjectInput
{
	public:
		PhysXRagdoll(PhysXCore * const pxManager);
		~PhysXRagdoll();

		void Load(const std::string &fileName);
		void Update(float deltaTime, Camera * camera);
		void Update(float deltaTime, int mods);
		void OnKeyPress(int key, int mods);
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods);

	private:
		Transform *transform;
		PhysXCore * const pxManager;
		unsigned int dynamicSphere;

	public:
		std::unordered_map<std::string, physx::PxActor*> actors;
		std::unordered_map<std::string, physx::PxD6Joint*> joints;
};

#endif