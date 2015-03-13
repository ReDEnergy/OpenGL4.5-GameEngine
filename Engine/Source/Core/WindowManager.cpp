//#include <pch.h>
#include "WindowManager.h"

#include <Core/Engine.h>
#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>

vector<WindowObject*> *WindowManager::windows = nullptr;

WindowObject* WindowManager::NewWindow(char* name, glm::ivec2 resolution, glm::ivec2 position, bool reshapable)
{
	WindowObject *W = new WindowObject();
	W->Init(name, resolution, position, reshapable);
	W->WindowMode();

	windows->push_back(W);
	return W;
}

void WindowManager::Init() {
	Manager::Debug->InitManager("WindowManager");
	WindowManager::windows = new vector<WindowObject*>;
}

void WindowManager::OnResize(GLFWwindow *W, int width, int height) {
	WindowObject *Wobj = GetWindowObject(W);
	Wobj->SetSize(width, height);
	glViewport(0, 0, width, height);
}

WindowObject* WindowManager::GetWindowObject(GLFWwindow* W) {
	for (auto window : *windows) {
		if (window->window == W)
			return window;
	}
	assert(false);
	return nullptr;
}