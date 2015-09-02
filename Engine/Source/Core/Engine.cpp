//#include <pch.h>
#include "Engine.h"
#include <Core/InputSystem.h>
#include <Core/WindowManager.h>
#include <Core/WindowObject.h>
#include <Core/World.h>


// Engine
double Engine::elapsedTime = 0;
double Engine::deltaTime = 0;
bool Engine::paused = false;
World* Engine::world = nullptr;
WindowObject* Engine::Window = nullptr;

/* Get elapsed time in seconds from when engine start */
void Engine::ComputeFrameDeltaTime() {
	static double lastTime = glfwGetTime() - 60.0/1000;

	elapsedTime = glfwGetTime();
	deltaTime =  elapsedTime - lastTime;
	lastTime = elapsedTime;
}

void Engine::SetWorldInstance(World *world_instance) {
	world = world_instance;
}

void Engine::Run() {

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(Window->window)) {
		Update();
	}

	Exit();
}

// TODO - add StartFrame / EndFrame
void Engine::Update()
{
	/* Poll and process events */
	glfwPollEvents();

	ComputeFrameDeltaTime();
	if (paused) return;

	world->FrameStart();

	/* Render Frame */
	if (world)
		world->Update((float)elapsedTime, (float)deltaTime);

	InputSystem::EndFrame();

	/* Swap front and back buffers */
	if (!paused)
		glfwSwapBuffers(Window->window);

	world->FrameEnd();
}

void Engine::Pause() {
	paused = !paused;
	cout << "RENDERING: " << paused << endl;
}

void Engine::Exit() {
	if (!glfwWindowShouldClose(Window->window)) {
		glfwSetWindowShouldClose(Window->window, 1);
		return;
	}
	cout << "=====================================================" << endl;
	cout << "Engine closed. Exit" << endl;
	glfwTerminate();
}

// -------------------------------------------------------------------------
// DEBUGGING AND LOGGING FUNCTIONS
// -------------------------------------------------------------------------
void ErrorCallback(int error, const char* description) {
	cout << error << " " << description;
}

void Engine::Init() {

	/* Initialize the library */
	if (!glfwInit())
		return;

	glfwSetErrorCallback(ErrorCallback);

	glClearDepth(1);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}