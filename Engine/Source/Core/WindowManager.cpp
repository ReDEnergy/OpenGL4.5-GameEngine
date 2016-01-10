//#include <pch.h>
#include "WindowManager.h"

#include <Core/Engine.h>
#include <Core/WindowObject.h>

#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>

vector<WindowObject*> *WindowManager::windows = nullptr;

WindowObject* WindowManager::Create(WindowProperties &properties)
{
	auto W = new WindowObject(properties);
	windows->push_back(W);
	return W;
}

void WindowManager::Init()
{
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