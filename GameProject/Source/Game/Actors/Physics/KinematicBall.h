#pragma once
#include <Core/GameObject.h>

#include <PhysX/PxPhysicsAPI.h>

using namespace physx;

class KinematicBall
	: public GameObject
{
	public:
		KinematicBall(float radius);
		~KinematicBall();

		void SetPosition(glm::vec3 pos);
		void SetPositionAndRotation(glm::vec3 pos, glm::quat rot);
		void Update();

	private:
		PxRigidDynamic *pxBody;
};

