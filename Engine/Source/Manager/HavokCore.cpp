//#include <pch.h>
#include "HavokCore.h"

#include <cstdio>

#include <include/havok.h>

#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>

// Math and base includes
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>

// Visual Debugger includes
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics2012/Utilities/VisualDebugger/hkpPhysicsContext.h>				

// Loading Havok Files
#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Common/Serialize/Util/hkStructureLayout.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Physics2012/Utilities/Serialize/hkpPhysicsData.h>

// Headers about collision
#include <Physics2012/Dynamics/World/hkpSimulationIsland.h>
#include <Physics2012/Collide/Dispatch/hkpAgentRegisterUtil.h>
#include <Physics2012/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics2012/Collide/Shape/hkpShapeContainer.h>
#include <Physics2012/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>

// Headers about collision
#include <Physics2012/Dynamics/World/hkpWorld.h>
#include <Physics2012/Dynamics/Entity/hkpEntity.h>
#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>

// System
#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>

// Havok
#include <Common/Base/keycode.cxx>

// This excludes libraries that are not going to be linked
// from the project configuration, even if the keycodes are present

#ifdef HK_FEATURE_PRODUCT_AI
#undef HK_FEATURE_PRODUCT_AI
#endif
#ifdef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_ANIMATION
#endif
#ifdef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_CLOTH
#endif
#ifdef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#endif
#ifdef HK_FEATURE_PRODUCT_DESTRUCTION_2012
#undef HK_FEATURE_PRODUCT_DESTRUCTION_2012
#endif
#ifdef HK_FEATURE_PRODUCT_BEHAVIOR
#undef HK_FEATURE_PRODUCT_BEHAVIOR
#endif
#ifdef HK_FEATURE_PRODUCT_MILSIM
#undef HK_FEATURE_PRODUCT_MILSIM
#endif

// #define HK_FEATURE_REFLECTION_PHYSICS
#define HK_EXCLUDE_FEATURE_MemoryTracker
#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
#define HK_EXCLUDE_LIBRARY_hkGeometryUtilities

// Keycode
#include <Common/Base/Config/hkProductFeatures.cxx>

using namespace std;

HavokCore::HavokCore()
{
	_vdbEnabled = false;

	#ifdef _DEBUG
	_vdbEnabled = true;
	#endif
}

HavokCore::~HavokCore()
{
	DeinitHavok();
}

void HavokCore::Init()
{

	Manager::Debug->InitManager("Havok");

	InitMemory();
	InitPhysicsWorld();

	if (_vdbEnabled)
		InitVDB();

	// Finished modifying the world
	_pWorld->unmarkForWrite();
}

void HK_CALL errorReport(const char* msg, void* userArgGivenToInit)
{
	printf("%s\n", msg);
}


void HavokCore::InitMemory()
{
	#if defined(HK_COMPILER_HAS_INTRINSICS_IA32) && HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
	// Flush all denormal/subnormal numbers (2^-1074 to 2^-1022) to zero.
	// Typically operations on denormals are very slow, up to 100 times slower than normal numbers.
		_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	#endif

	// Initialize the base system including our memory system
	// Allocate 10MB of physics solver buffer.
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo( 10 * 1000 * 1024 ) );
	hkBaseSystem::init( memoryRouter, errorReport );


	// We can cap the number of threads used - here we use the maximum for whatever multithreaded platform we are running on. This variable is
	// set in the following code sections.
	int totalNumThreadsUsed;

	// Get the number of physical threads available on the system
	hkHardwareInfo hwInfo;
	hkGetHardwareInfo(hwInfo);
	totalNumThreadsUsed = hwInfo.m_numThreads;

	// We use one less than this for our thread pool, because we must also use this thread for our simulation
	hkCpuJobThreadPoolCinfo threadPoolCinfo;
	threadPoolCinfo.m_numThreads = totalNumThreadsUsed - 1;

	// This line enables timers collection, by allocating 200 Kb per thread.  If you leave this at its default (0),
	// timer collection will not be enabled.
	threadPoolCinfo.m_timerBufferPerThreadAllocation = 200000;
	_threadPool = new hkCpuJobThreadPool( threadPoolCinfo );

	// We also need to create a Job queue. This job queue will be used by all Havok modules to run multithreaded work.
	// Here we only use it for physics.
	hkJobQueueCinfo info;
	info.m_jobQueueHwSetup.m_numCpuThreads = totalNumThreadsUsed;
	_jobQueue = new hkJobQueue(info);

	// Enable monitors for this thread.
	// Monitors have been enabled for thread pool threads already (see above comment).
	hkMonitorStream::getInstance().resize(200000);
}

void HavokCore::InitPhysicsWorld()
{
	_pWorldInfo = new hkpWorldCinfo();
	_pWorldInfo->m_contactPointGeneration = hkpWorldCinfo::CONTACT_POINT_ACCEPT_ALWAYS;
	_pWorldInfo->m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;
	_pWorldInfo->m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY;
	_pWorldInfo->setBroadPhaseWorldSize(1000);

	_pWorld = new hkpWorld(*_pWorldInfo);

	// DEBUG: Disable deactivation, view in VDB.
	_pWorld->m_wantDeactivation = false;

	// When the simulation type is SIMULATION_TYPE_MULTITHREADED, in the debug build, the sdk performs checks
	// to make sure only one thread is modifying the world at once to prevent multithreaded bugs. Each thread
	// must call markForRead / markForWrite before it modifies the world to enable these checks.
	_pWorld->markForWrite();

	// Register all collision agents.
	// It's important to register collision agents before adding any entities to the world.
	hkpAgentRegisterUtil::registerAllAgents( _pWorld->getCollisionDispatcher() );

	// We need to register all modules we will be running multi-threaded with the job queue.
	_pWorld->registerWithJobQueue( _jobQueue );
}

// Initialise the Visual Debugger
void HavokCore::InitVDB()
{
	hkArray<hkProcessContext*> contexts;

	_context = new hkpPhysicsContext();
	hkpPhysicsContext::registerAllPhysicsProcesses(); // all the physics viewers
	_context->addWorld(_pWorld); // add the physics world so the viewers can see it
	contexts.pushBack(_context);

	_vdb = new hkVisualDebugger(contexts);
	_vdb->serve();
}

// Deinitialise Havok
void HavokCore::DeinitHavok()
{
	// Cleanup physics
	_pWorld->markForWrite();
	_pWorld->removeReference();

	delete _jobQueue;

	// Clean up the thread pool
	_threadPool->removeReference();

	if (_vdbEnabled)
		DeinitVDB();

	hkBaseSystem::quit();
    hkMemoryInitUtil::quit();
}

// Deinitialise the Visual Debugger
void HavokCore::DeinitVDB()
{
	_vdb->removeReference();
	_context->removeReference();
	_vdbEnabled = false;
}

void HavokCore::StepSimulation(float deltaTime)
{
	UpdateWorld();
	StepPhysicsSimulation(deltaTime);

	if (_vdbEnabled)
		StepVDBSimulation();

	// Clear accumulated timer data in this thread and all slave threads
	hkMonitorStream::getInstance().reset();
	_threadPool->clearTimerData();
}

void HavokCore::StepPhysicsSimulation(float deltaTime)
{
	// Step the physics world. This single call steps using this thread and all threads
	// in the threadPool. For other products you add jobs, call process all jobs and wait for completion.
	// See the multithreading chapter in the user guide for details.
	_pWorld->stepMultithreaded( _jobQueue, _threadPool, 0.016f);
}

void HavokCore::StepVDBSimulation()
{
	_context->syncTimers(_threadPool);
	_vdb->step();
}

const hkpWorld* const HavokCore::GetWorld() const {
	return _pWorld;
}

void HavokCore::MarkForRead() {
	_pWorld->markForRead();
}

void HavokCore::UnmarkForRead() {
	_pWorld->unmarkForRead();
}

void HavokCore::MarkForWrite() {
	_pWorld->markForWrite();
}

void HavokCore::UnmarkForWrite() {
	_pWorld->unmarkForWrite();
}

void HavokCore::AddIntoTheWorld(hkpEntity *entity) {
	toAdd.push_back(entity);
}

void HavokCore::RemoveFromTheWorld(hkpEntity *entity) {
	toRemove.push_back(entity);
}

void HavokCore::UpdateWorld() {
	bool shouldAdd = !toAdd.empty();
	bool shouldRemove = !toRemove.empty();

	if (shouldAdd || shouldRemove) {
		MarkForWrite();

		if (shouldAdd) {
			for (auto entity: toAdd) {
				_pWorld->addEntity(entity);
				entity->removeReference();
			}
			toAdd.clear();
		}

		if (shouldRemove) {
			for (auto entity: toRemove) {
				_pWorld->removeEntity(entity);
			}
			toRemove.clear();
		}

		UnmarkForWrite();
	}
}
