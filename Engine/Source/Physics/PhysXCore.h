#pragma once

#ifdef PHYSICS_ENGINE

#ifdef ENGINE_DLL_EXPORTS
#include <PxPhysicsAPI.h>
#endif

#include <include/dll_export.h>

class DLLExport PhysXCore
{
	friend class Physics;

	protected:
		PhysXCore();
		~PhysXCore();

	public:
		void Init();
		void StepPhysics(float deltaTime);
		void FetchResults() const;
		void CleanupPhysics();

	#ifdef ENGINE_DLL_EXPORTS
		void AddActorToScene(physx::PxActor &actor);
		void AddCollection(physx::PxCollection &collection);
		void RemoveActorFromScene(physx::PxActor &actor);

		physx::PxDefaultAllocator*		GetPxAlocator();
		physx::PxCooking*				GetPxCooking() const;
		physx::PxPhysics*				GetPhysics() const;
		physx::PxScene*					GetPxScene() const;

	private:
		void CreateScene();

	private:
		physx::PxDefaultAllocator		gAllocator;
		physx::PxDefaultErrorCallback	gErrorCallback;

		physx::PxFoundation				*gFoundation;
		physx::PxPhysics				*gPhysics;
		physx::PxCooking				*gCooking;

		physx::PxDefaultCpuDispatcher	*gDispatcher;
		physx::PxScene					*gScene;

		physx::PxVisualDebuggerConnection *gConnection;
	#endif
};
#endif // PHYSICS_ENGINE