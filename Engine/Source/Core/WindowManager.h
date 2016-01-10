#pragma once
#include <vector>

#include <include/gl.h>
#include <include/glm.h>

class WindowObject;
struct WindowProperties;

using namespace std;

class WindowManager
{
	private:
		WindowManager() {};
		~WindowManager() {};

	public:
		static void Init();
		static void OnResize(GLFWwindow *W, int width, int height);
		static WindowObject* Create(WindowProperties & properties);
		static WindowObject* GetWindowObject(GLFWwindow *W);

	private:
		static vector<WindowObject*> *windows;
};