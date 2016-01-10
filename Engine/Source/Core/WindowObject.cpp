//#include <pch.h>
#include "WindowObject.h"

#include <Core/InputSystem.h>
#include <Core/WindowManager.h>

WindowObject::WindowObject(WindowProperties & properties)
{
	coreContext = true;

	prop.name		= properties.name;
	prop.hidden		= properties.hidden;
	prop.resolution	= properties.resolution;
	prop.position	= properties.position;
	prop.resizable  = properties.resizable;
	prop.fullScreen = properties.fullScreen;
	prop.centered	= properties.centered;

	resolution = properties.resolution;
	totalPixels = resolution.x * resolution.y;
	aspectRatio = float(resolution.x) / resolution.y;
	center = resolution / 2;

	allowedControl = !prop.hidden;
	prop.fullScreen ? FullScreen() : WindowMode();
}

void WindowObject::FullScreen()
{
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *videoDisplay = glfwGetVideoMode(monitor);
	window = glfwCreateWindow(videoDisplay->width, videoDisplay->height, prop.name, monitor, NULL);
	glfwMakeContextCurrent(window);
	SetSize(videoDisplay->width, videoDisplay->height);
	SetWindowCallbacks();
}

void WindowObject::WindowMode()
{
	if (prop.hidden) {
		glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	if (coreContext)
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(resolution.x, resolution.y, prop.name, NULL, NULL);
	glfwMakeContextCurrent(window);
	if (prop.centered) {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *videoDisplay = glfwGetVideoMode(monitor);
		auto screenSize = glm::ivec2(videoDisplay->width, videoDisplay->height);
		auto offset = (screenSize - prop.resolution) / 2;
		glfwSetWindowPos(window, offset.x, offset.y);
	}
	else {
		glfwSetWindowPos(window, prop.position.x, prop.position.y);
	}
	SetSize(resolution.x, resolution.y);
	SetWindowCallbacks();

	// Save native context data
	nativeWGLContext = glfwGetWGLContext(window);
	hdc = wglGetCurrentDC();
}

void WindowObject::SetWindowCallbacks()
{
	glfwSetWindowSizeCallback(window, WindowManager::OnResize);
	glfwSetKeyCallback(window, InputSystem::KeyCallback);
	glfwSetMouseButtonCallback(window, InputSystem::MouseClick);
	glfwSetCursorPosCallback(window, InputSystem::CursorMove);
}

void WindowObject::SetSize(int width, int height)
{
	if (!allowedControl) return;

	resolution = glm::ivec2(width, height);
	totalPixels = width * height;
	center = resolution / 2;
	aspectRatio = float(width) / height;
	glViewport(0, 0, width, height);
}

void WindowObject::SetContext()
{
	// WHY IS THIS NOT WORKING ?! Maybe because of HDC!
	//	glfwMakeContextCurrent(window);

	wglMakeCurrent(hdc, nativeWGLContext);
}

bool WindowObject::IsCoreContext() const
{
	return coreContext;
}

unsigned int WindowObject::GetResolution() const
{
	return totalPixels;
}

// Clip user cursor inside the Window
// ClipCursor function use from Window.h
void WindowObject::ClipPointer(bool state)
{
	if (!allowedControl) return;

	int clippingEdge = 5;
	cursorClip = state;
	if (state == false) {
		ClipCursor(0);
		return;
	}

	int posX, posY;
	glfwGetWindowPos(window, &posX, &posY);
	WindowRECT.left = posX + clippingEdge;
	WindowRECT.top = posY + clippingEdge;
	WindowRECT.bottom = WindowRECT.top + resolution.y - 2 * clippingEdge;
	WindowRECT.right = WindowRECT.left + resolution.x - 2 * clippingEdge;

	ClipCursor(&WindowRECT);
}

void WindowObject::HidePointer(bool state)
{
	if (!allowedControl) return;

	hiddenPointer = state;
	glfwSetInputMode(window, GLFW_CURSOR, hiddenPointer ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
}

void WindowObject::AllowControl(bool control)
{
	allowedControl = control;
}

void WindowObject::SetPointerPosition(glm::ivec2 position)
{
	if (!allowedControl) return;

	if (cursorClip) {
		pointerPos = center;
		glfwSetCursorPos(window, center.x, center.y);
	}
	else {
		pointerPos = position;
		glfwSetCursorPos(window, position.x, position.y);
	}
}
