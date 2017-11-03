#include "Engine.h"

#include <iostream>

#include <include/gl.h>

#include <Core/InputSystem.h>
#include <Core/WindowManager.h>
#include <Core/WindowObject.h>
#include <Core/World.h>

using namespace std;

void Engine::Exit()
{
	cout << "=====================================================" << endl;
	cout << "Engine closed. Exit" << endl;
	glfwTerminate();
}

double Engine::GetElapsedTime()
{
	return glfwGetTime();
}

void Engine::Init()
{
	/* Initialize the library */
	if (!glfwInit())
		exit(0);
}