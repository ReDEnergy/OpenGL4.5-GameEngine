#include "WindowManager.h"

#include <iostream>

#include <Core/Engine.h>
#include <Core/WindowObject.h>

#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>

#include <include/gl.h>

using namespace std;

vector<WindowObject*> WindowManager::windowList;
unordered_map<string, WindowObject*> WindowManager::windows;

WindowObject* WindowManager::sharedContextWindow = nullptr;

WindowManager::~WindowManager()
{
}

void WindowManager::Init()
{
	WindowManager::windows.clear();
	sharedContextWindow = nullptr;
}

WindowObject* WindowManager::GetShaderdWindowContext()
{
	return sharedContextWindow;
}

void WindowManager::OnGLFWClose(GLFWwindow * W)
{
	WindowObject *Wobj = GetWindowObject(W);
	Wobj->Close();
}

void WindowManager::OnGLFWResize(GLFWwindow *W, int width, int height)
{
	WindowObject *Wobj = GetWindowObject(W);
	Wobj->SetSize(width, height);
}

void WindowManager::OnGLFWError(int error, const char * description)
{
	cout << "[GLFW ERROR]\t" << error << "\t" << description << endl;
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

	if (W == nullptr)
		return windowList[0];
	
	for (auto window : windowList) {
		if (window->window == W)
			return window;
	}

	return windowList[0];
}

WindowObject* WindowManager::DetachCurrentContext()
{
	auto glfwWindow = glfwGetCurrentContext();
	if (glfwWindow != nullptr)
	{
		glfwMakeContextCurrent(NULL);
		for (auto window : windowList)
		{
			if (window->window == glfwWindow)
			{
				return window;
			}
		}
	}
	return nullptr;
}
