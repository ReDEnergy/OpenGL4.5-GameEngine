#pragma once
#include <list>

#include <include/dll_export.h>

class hkpWorld;
class hkpEntity;
class hkpPhysicsContext;
class hkVisualDebugger;
class hkJobQueue;
class hkpWorldCinfo;
class hkJobThreadPool;

class DLLExport HavokCore
{
	protected:
		HavokCore();
		~HavokCore();

	private:
		void InitMemory();
		void InitPhysicsWorld();
		void InitVDB();

		void DeinitHavok();
		void DeinitVDB();

		void StepPhysicsSimulation(float deltaTime);
		void StepVDBSimulation();
		void UpdateWorld();

	public:
		const hkpWorld* const GetWorld() const;
		void StepSimulation(float deltaTime);

		void AddEntity(hkpEntity* entity);

		void Init();
		void MarkForRead();
		void UnmarkForRead();
		void MarkForWrite();
		void UnmarkForWrite();
		void AddIntoTheWorld(hkpEntity *entity);
		void RemoveFromTheWorld(hkpEntity *entity);

	public:
		// Make it private
		hkpWorld* _pWorld; // Physics world.

	private:
		bool _vdbEnabled; // vdb = Visual Debugger.
		hkpPhysicsContext* _context;
		hkVisualDebugger* _vdb;

		hkJobQueue* _jobQueue;
		hkJobThreadPool* _threadPool;

		hkpWorldCinfo* _pWorldInfo; // Info about global simulation parameters.

		std::list<hkpEntity*> toAdd;
		std::list<hkpEntity*> toRemove;
};
