#pragma once
#include <Windows.h>

#include <include/gl.h>
#include <include/glm.h>

/*
 * Class WindowObject
 */

class WindowObject
{
	friend class WindowManager;

	public:
		WindowObject();
		WindowObject(char* name, glm::ivec2 resolution, glm::ivec2 position, bool reshapable);
		~WindowObject();

		void FullScreen();
		void WindowMode();
		glm::ivec2 GetCenter();
		void ClipPointer(bool state);
		void SetSize(int width, int height);
		void Init(char* name, glm::ivec2 resolution, glm::ivec2 position, bool reshapable);

	private:
		void SetWindowCallbacks();

	public:
		glm::ivec2 resolution;
		float aspectRatio;
		GLFWwindow* window;

	private:
		char* name;
		glm::ivec2 position;
		glm::ivec2 center;
		int positionX;
		int positionY;
		bool reshapable;
		RECT WindowRECT;
};

//case GLFW_KEY_F6:
//{
//	clipCursor = !clipCursor;
//	ClipWindowCursor(clipCursor);
//	cout << "CLIP CURSOR: " << clipCursor << endl;
//	break;
//}

//case GLFW_KEY_F7:
//{
//	hideCursor = !hideCursor;
//	hideCursor ? glfwSetInputMode(W, GLFW_CURSOR, GLFW_CURSOR_HIDDEN) : glfwSetInputMode(W, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//	cout << "CURSOR: " << hideCursor << endl;
//	break;
//}

//case GLFW_KEY_F11:
//{
//	fullScreen = !fullScreen;
//	cout << "FullScreen: " << fullScreen << endl;
//	fullScreen ? EnterFullScreen() : EnterWindowMode();
//	ClipWindowCursor(clipCursor);
//}

