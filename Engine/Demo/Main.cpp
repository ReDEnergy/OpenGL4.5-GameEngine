#include <ctime>
#include <iostream>

using namespace std;

#include <Core/Engine.h>
#include <Core/World.h>
#include <Core/WindowObject.h>
#include <Core/WindowManager.h>

#include <Manager/Manager.h>

#include "SimpleScene.h"

int main(int argc, char **argv)
{
	srand((unsigned int)time(NULL));

	Engine::Init();
	Manager::Init();

	auto window = WindowManager::GetDefaultWindow();
	window->SetVSync(true);

	auto simpleScene = new SimpleScene();
	simpleScene->InitResources(window);
	simpleScene->Init();
	simpleScene->Run();

	// Signals to the Engine to release the OpenGL context
	Engine::Exit();

	return 0;
}