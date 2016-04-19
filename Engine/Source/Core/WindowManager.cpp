#include "WindowManager.h"

#include <include/gl.h>
#include <Core/Engine.h>
#include <Core/WindowObject.h>

#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>

using namespace std;

vector<WindowObject*> WindowManager::windowList;
unordered_map<string, WindowObject*> WindowManager::windows;

WindowObject* WindowManager::sharedContextWindow = nullptr;

void WindowManager::Init()
{
	Manager::Debug->InitManager("WindowManager");
	WindowManager::windows.clear();
	sharedContextWindow = nullptr;
}

void WindowManager::OnClose(GLFWwindow * W)
{
	WindowObject *Wobj = GetWindowObject(W);
	Wobj->Close();
}

WindowObject* WindowManager::GetShaderdWindowContext()
{
	return sharedContextWindow;
}

void WindowManager::RegisterWindow(WindowObject *W)
{
	windowList.push_back(W);
	windows[W->props.name] = W;
	if (sharedContextWindow == nullptr && W->props.IsSharedContext()) {
		sharedContextWindow = W;
	}
}

WindowObject * WindowManager::GetDefaultWindow()
{
	return windowList[0];
}

WindowObject* WindowManager::GetWindowObjectByName(const char* name)
{
	if (name == nullptr)
		return windowList[0];

	if (windows.find(name) != windows.end())
		return windows[name];
	return windowList[0];
}

WindowObject* WindowManager::GetWindowObject(GLFWwindow* W)
{
	if (windowList.empty())
		return nullptr;
	
	for (auto window : windowList) {
		if (window->window == W)
			return window;
	}
	return windowList[0];
}

void WindowManager::OnResize(GLFWwindow *W, int width, int height)
{
	WindowObject *Wobj = GetWindowObject(W);
	Wobj->SetSize(width, height);
	glViewport(0, 0, width, height);
}