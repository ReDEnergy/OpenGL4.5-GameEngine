#pragma once
#include <Windows.h>

#include <include/dll_export.h>
#include <include/gl.h>
#include <include/glm.h>

/*
 * Class WindowObject
 */

class DLLExport WindowObject
{
	friend class WindowManager;

	public:
		WindowObject();
		WindowObject(char* name, glm::ivec2 resolution, glm::ivec2 position, bool reshapable);
		~WindowObject();

		void FullScreen();
		void WindowMode();
		void ClipPointer(bool state);
		void HidePointer(bool state);

		void SetPointerPosition(glm::ivec2 position);
		void SetSize(int width, int height);
		void SetContext();

		void Init(char* name, glm::ivec2 resolution, glm::ivec2 position, bool reshapable);

	private:
		void SetWindowCallbacks();

	public:
		float aspectRatio;
		glm::ivec2 resolution;
		glm::ivec2 pointerPos;
		GLFWwindow* window;

	private:
		char* name;
		glm::ivec2 position;
		glm::ivec2 center;
		bool reshapable;
		bool hiddenPointer;
		bool cursorClip;
		RECT WindowRECT;
		
		// Native Opengl context
		HDC hdc;
		HGLRC nativeWGLContext;

};