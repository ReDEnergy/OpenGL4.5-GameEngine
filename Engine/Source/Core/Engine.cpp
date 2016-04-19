#include "Engine.h"

#include <iostream>

#include <include/gl.h>

#include <Core/InputSystem.h>
#include <Core/WindowManager.h>
#include <Core/WindowObject.h>
#include <Core/World.h>

using namespace std;

// Engine
double Engine::elapsedTime = 0;
float Engine::deltaTime = 0;
bool Engine::paused = false;
World* Engine::world = nullptr;
WindowObject* Engine::Window = nullptr;

/* Get elapsed time in seconds from when engine start */
void Engine::ComputeFrameDeltaTime()
{
	static double lastTime = glfwGetTime() - 60.0/1000;

	elapsedTime = glfwGetTime();
	deltaTime =  float(elapsedTime - lastTime);
	lastTime = elapsedTime;
}

void Engine::SetWorldInstance(World *world_instance)
{
	world = world_instance;
}

float Engine::GetLastFrameTime()
{
	return deltaTime;
}

float Engine::GetElapsedTime()
{
	return (float)glfwGetTime();
}

void Engine::Run()
{
	/* Loop until the user closes the window */
	while (!Window->ShouldClose())
	{
		Update();
	}

	Exit();
}

// TODO - add StartFrame / EndFrame
void Engine::Update()
{
	/* Poll and process events */
	Window->PollEvents();

	ComputeFrameDeltaTime();
	if (paused) return;

	/* Frame */
	if (world) {
		world->FrameStart();
		world->Update((float)deltaTime);
		world->FrameEnd();
	}

	/* Swap front and back buffers */
	if (!paused)
		Window->SwapBuffers();
}

void Engine::Pause()
{
	paused = !paused;
	cout << "RENDERING: " << paused << endl;
}

void Engine::Exit()
{
	if (!Window->ShouldClose())
	{
		Window->Close();
		return;
	}
	cout << "=====================================================" << endl;
	cout << "Engine closed. Exit" << endl;
	glfwTerminate();
}

// -------------------------------------------------------------------------
// DEBUGGING AND LOGGING FUNCTIONS
// -------------------------------------------------------------------------
void ErrorCallback(int error, const char* description)
{
	cout << "[GLFW ERROR]\t" << error << "\t" << description << endl;
}

void Engine::Init()
{
	/* Initialize the library */
	if (!glfwInit())
		return;

	glfwSetErrorCallback(ErrorCallback);
	CheckOpenGLError();
}