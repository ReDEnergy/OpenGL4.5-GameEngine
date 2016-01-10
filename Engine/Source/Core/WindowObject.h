#pragma once
#include <Windows.h>

#include <include/dll_export.h>
#include <include/gl.h>
#include <include/glm.h>


struct WindowProperties
{
	const char* name;
	glm::ivec2 resolution;
	glm::ivec2 position;
	bool resizable;
	bool hidden;
	bool fullScreen;
	bool centered;
};

/*
 * Class WindowObject
 */

class DLLExport WindowObject
{
	friend class WindowManager;

	public:
		WindowObject(WindowProperties &properties);
		~WindowObject();

		void FullScreen();
		void WindowMode();
		void ClipPointer(bool state);
		void HidePointer(bool state);
		void AllowControl(bool control);

		void SetPointerPosition(glm::ivec2 position);
		void SetSize(int width, int height);
		void SetContext();

		bool IsCoreContext() const;

		// Returns the total number of pixels
		unsigned int GetResolution() const;

	private:
		void SetWindowCallbacks();

	public:
		float aspectRatio;
		glm::ivec2 resolution;
		glm::ivec2 pointerPos;
		GLFWwindow* window;

	private:
		unsigned int totalPixels;

		WindowProperties prop;
		glm::ivec2 center;
		bool coreContext;
		bool allowedControl;
		bool hiddenPointer;
		bool cursorClip;
		RECT WindowRECT;
		
		// Native Opengl context
		HDC hdc;
		HGLRC nativeWGLContext;

};