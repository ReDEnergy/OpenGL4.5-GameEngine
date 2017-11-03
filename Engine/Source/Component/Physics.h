#pragma once
#include <include/glm.h>
#include <include/dll_export.h>

class GameObject;
class Transform;

#ifdef PHYSICS_ENGINE

namespace physx
{
	class PxShape;
	class PxActor;
	class PxRigidActor;
	class PxRigidBody;
	class PxRigidStatic;
	class PxRigidDynamic;
}

#endif

class DLLExport Physics
{
	friend class PhsyXEventCallback;

	public:
		Physics(GameObject *object);
		Physics(Physics &physics);
		~Physics();

		void SetGameObject(GameObject *GO);

		void UpdateScale();

		virtual bool HasGravity() const final;
		virtual bool IsDynamic() const final;
		virtual bool IsKinematic() const final;
		virtual bool IsTrigger() const final;

		virtual void SetRigidBody();
		virtual void SetGravity(bool state);
		virtual void SetMass(float mass);
		virtual void SetIsDynamic(bool state);
		virtual void SetIsKinematic(bool state);
		virtual void SetIsTrigger(bool trigger);
		virtual void SetSimulationState(bool state);
		virtual void SetTransform();

		virtual void Update();
		virtual bool IsActive() const;

		void UpdatePhysicsEngine();
		void SetLinearVelocity(glm::vec3 velocity);

	protected:
		void OnTrigger(Physics *other);

	private:
		void Init();
		virtual void ReadTransform() final;

	private:
		bool applyGravity;
		bool isDynamic;
		bool isTrigger;
		bool isKinematic;

		GameObject *GO;

		#ifdef PHYSICS_ENGINE
		physx::PxShape *shape;
		physx::PxRigidActor *actor;
		physx::PxRigidStatic *staticBody;
		physx::PxRigidDynamic *dynamicBody;
		#endif
};
