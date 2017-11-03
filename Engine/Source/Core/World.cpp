#include "World.h"

#include <Core/WindowObject.h>
#include <Core/Engine.h>

World::World()
{
	previousTime = 0;
	elapsedTime = 0;
	deltaTime = 0;
	paused = false;
	window = nullptr;
	shouldClose = false;

	Init();
}

void World::Init()
{
}

void World::Run()
{
	while (!shouldClose)
	{
		LoopUpdate();
	}
}

void World::Pause()
{
	paused = !paused;
}

void World::Exit()
{
	shouldClose = true;
}

double World::GetLastFrameTime()
{
	return deltaTime;
}

void World::ComputeFrameDeltaTime()
{
	elapsedTime = Engine::GetElapsedTime();
	deltaTime = elapsedTime - previousTime;
	previousTime = elapsedTime;
}

void World::LoopUpdate()
{
	/* Poll and process events */
	window->PollEvents();

	ComputeFrameDeltaTime();
	if (paused) return;

	/* Frame */
	if (world) {
		world->FrameStart();
		world->Update(static_cast<float>(deltaTime));
		world->FrameEnd();
	}

	/* Swap front and back buffers */
	if (!paused)
		window->SwapBuffers();
}
