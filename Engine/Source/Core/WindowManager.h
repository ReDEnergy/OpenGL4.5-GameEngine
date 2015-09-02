#pragma once
#include <vector>

#include <include/gl.h>
#include <include/glm.h>

class WindowObject;

using namespace std;

class WindowManager
{
	private:
		WindowManager() {};
		~WindowManager() {};

	public:
		static void Init();
		static void OnResize(GLFWwindow *W, int width, int height);
		static WindowObject* NewWindow(char* name, glm::ivec2 resolution, glm::ivec2 position, bool reshapable);
		static WindowObject* GetWindowObject(GLFWwindow *W);

	private:
		static vector<WindowObject*> *windows;
};