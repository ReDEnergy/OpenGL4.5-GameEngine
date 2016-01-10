#pragma once
#include <Core/GameObject.h>
#include <Event/EventListener.h>

#include <PhysX/PxPhysicsAPI.h>

using namespace physx;

class PhysicsBall
	: public GameObject
	, public EventListener
{
	public:
		PhysicsBall(float radius, glm::vec3 pos = glm::vec3(), glm::vec3 direction = glm::vec3());
		~PhysicsBall();

		void Update();

	private:
		void OnEvent(EventType Event, void *data);
		TimerEvent<EventType> *timer;

	private:
		PxRigidDynamic *pxBody;
};

