#include "WindowObject.h"

#include <iostream>
#include <functional>

#include <include/gl.h>
#include <include/glm_utils.h>
#include <include/utils.h>

#include <Core/Engine.h>
#include <Core/InputSystem.h>
#include <Core/WindowManager.h>
#include <Component/ObjectInput.h>

#include <include/gl_native.h>

using namespace std;
using namespace std::placeholders;

WindowProperties::WindowProperties(bool shareContext)
	: sharedContext(shareContext)
{
	name = "WindowName";
	resolution = glm::ivec2(1280, 720);
	aspectRatio = 1280.0f / 720.0f;
	cursorPos = resolution / 2;
	position = glm::ivec2(0);
	resizable = true;
	centered = true;
	fullScreen = false;
	visible = true;
	hideOnClose = false;
	vSync = false;
}

bool WindowProperties::IsSharedContext() const
{
	return sharedContext;
}

WindowObject::WindowObject(WindowProperties properties)
	: props(std::move(properties))
{
	frameID = 0;
	deltaFrameTime = 0;
	useNativeHandles = false;
	props.aspectRatio = float(props.resolution.x) / props.resolution.y;

	// Init OpenGL Window
	InitWindow();
	//SetVSync(props.vSync);

	CheckOpenGLError();

	// Set default state
	mouseMoved = false;
	scrollEvent = false;

	mouseButtonAction = 0;
	mouseButtonStates = 0;
	registeredKeyEvents = 0;
	memset(keyStates, 0, 384);
	memset(keyScanCode, 0, 512);

	SetWindowCallbacks();

	// Register window
	WindowManager::RegisterWindow(this);
}

WindowObject::~WindowObject()
{
	glfwDestroyWindow(window);
}

void WindowObject::Show()
{
	props.visible = true;
	glfwShowWindow(window);
	MakeCurrentContext();
}

void WindowObject::Hide()
{
	props.visible = false;
	glfwHideWindow(window);
}

void WindowObject::SetVSync(bool state)
{
	props.vSync = state;
	#if defined(_WIN32) && !defined(OPENGL_ES)
	wglSwapIntervalEXT(state ? 1 : 0);
	#else
	glfwSwapInterval(state);
	#endif
}

bool WindowObject::ToggleVSync()
{
	SetVSync(!props.vSync);
	return props.vSync;
}

void WindowObject::Close()
{
	props.hideOnClose ? Hide() : glfwSetWindowShouldClose(window, 1);
}

int WindowObject::ShouldClose() const
{
	return glfwWindowShouldClose(window);
}

void WindowObject::ShowPointer()
{
	hiddenPointer = false;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void WindowObject::HidePointer()
{
	hiddenPointer = true;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void WindowObject::DisablePointer()
{
	hiddenPointer = true;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowObject::SetWindowPosition(glm::ivec2 position)
{
	props.position = position;
	glfwSetWindowPos(window, position.x, position.y);
}

void WindowObject::CenterWindow()
{
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	if (monitor != nullptr)
	{
		const GLFWvidmode *videoDisplay = glfwGetVideoMode(monitor);
		glm::ivec2 screenSize = glm::ivec2(videoDisplay->width, videoDisplay->height);

		props.centered = true;
		SetWindowPosition((screenSize - props.resolution) / 2);
	}
}

void WindowObject::CenterPointer()
{
	props.cursorPos.x = props.resolution.x / 2;
	props.cursorPos.y = props.resolution.y / 2;
	glfwSetCursorPos(window, props.cursorPos.x, props.cursorPos.y);
}

void WindowObject::SetPointerPosition(int mousePosX, int mousePosY)
{
	props.cursorPos.x = mousePosX;
	props.cursorPos.y = mousePosY;
	glfwSetCursorPos(window, mousePosX, mousePosY);
}

void WindowObject::PollEvents() const
{
	glfwPollEvents();
}

void WindowObject::ComputeFrameTime()
{
	frameID++;
	auto currentTime = Engine::GetElapsedTime();
	deltaFrameTime = currentTime - elapsedTime;
	elapsedTime = currentTime;
}

void WindowObject::InitWindow()
{
	window = nullptr;

	glfwWindowHint(GLFW_VISIBLE, props.visible);

	#ifdef OPENGL_ES
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
		glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, props.glContext.major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, props.glContext.minor);
	#else
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, props.glContext.major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, props.glContext.minor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#endif

	props.fullScreen ? FullScreen() : WindowMode();
}

void WindowObject::FullScreen()
{
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *videoDisplay = glfwGetVideoMode(monitor);

	window = glfwCreateWindow(videoDisplay->width, videoDisplay->height, props.name.c_str(), monitor, NULL);
	assert(window != nullptr);

	glfwMakeContextCurrent(window);
	CheckOpenGLError();

	SetSize(videoDisplay->width, videoDisplay->height);
}

void WindowObject::WindowMode()
{
	GLFWwindow *shaderCtxWindow = NULL;
	if (props.IsSharedContext()) {
		auto W = WindowManager::GetShaderdWindowContext();
		if (W) shaderCtxWindow = W->GetGLFWWindow();
	}

	window = glfwCreateWindow(props.resolution.x, props.resolution.y, props.name.c_str(), NULL, shaderCtxWindow);
	assert(window != nullptr);

	glfwMakeContextCurrent(window);
	CheckOpenGLError();

	#if defined(OPENGL_ES)
		EGLContext context = glfwGetEGLContext(window);
	#endif

	#if defined(_WIN32) && !defined(OPENGL_ES)
		openglHandle = wglGetCurrentDC();
		nativeRenderingContext = glfwGetWGLContext(window);
	#endif

	if (props.centered)
	{
		CenterWindow();
	}

	SetSize(props.resolution.x, props.resolution.y);
}

void WindowObject::SetWindowCallbacks()
{
	glfwSetWindowCloseCallback(window, WindowManager::OnGLFWClose);
	glfwSetWindowSizeCallback(window, WindowManager::OnGLFWResize);
	glfwSetKeyCallback(window, InputSystem::KeyCallback);
	glfwSetMouseButtonCallback(window, InputSystem::MouseClick);
	glfwSetCursorPosCallback(window, InputSystem::CursorMove);
	glfwSetScrollCallback(window, InputSystem::MouseScroll);
}

GLFWwindow * WindowObject::GetGLFWWindow() const
{
	return window;
}

bool WindowObject::KeyHold(int keyCode) const
{
	return keyStates[keyCode];
}

bool WindowObject::MouseHold(int button) const
{
	return IS_BIT_SET(mouseButtonStates, button);
}

int WindowObject::GetSpecialKeyState() const
{
	return keyMods;
}

void WindowObject::Subscribe(ObjectInput *IC)
{
	observers.push_back(IC);
}

void WindowObject::KeyCallback(int key, int scanCode, int action, int mods)
{
	keyMods = mods;
	bool keyState = action ? true : false;
	if (keyStates[key] != keyState)
	{
		keyStates[key] = keyState;
		keyEvents[registeredKeyEvents] = key;
		registeredKeyEvents++;
	}
}

void WindowObject::MouseButtonCallback(int button, int action, int mods)
{
	// Only button events and mods are kept
	// Mouse position is the current frame position
	keyMods = mods;
	SET_BIT(mouseButtonAction, button);
	action ? SET_BIT(mouseButtonStates, button) : CLEAR_BIT(mouseButtonStates, button);
}

void WindowObject::MouseMove(int posX, int posY)
{
	// Save information for processing later on the Update thread
	if (mouseMoved)
	{
		mouseDeltaX += posX - props.cursorPos.x;
		mouseDeltaY += posY - props.cursorPos.y;
	}
	else
	{
		mouseMoved = true;
		mouseDeltaX = posX - props.cursorPos.x;
		mouseDeltaY = posY - props.cursorPos.y;
	}
	props.cursorPos = glm::ivec2(posX, posY);
}

void WindowObject::MouseScroll(double offsetX, double offsetY)
{
	scrollEvent = true;
	scrollOffsetX = offsetX;
	scrollOffsetY = offsetY;
}

void WindowObject::UpdateObservers()
{
	ComputeFrameTime();

	// Mouse move event
	if (mouseMoved) {
		mouseMoved = false;
		for (auto obs : observers) {
			obs->OnMouseMove(props.cursorPos.x, props.cursorPos.y, mouseDeltaX, mouseDeltaY);
		}
	}

	// Mouse scroll event
	if (scrollEvent)
	{
		scrollEvent = false;
		for (auto obs : observers) {
			obs->OnMouseScroll(props.cursorPos.x, props.cursorPos.y, scrollOffsetX, scrollOffsetY);
		}
	}

	// Mouse button press event
	auto pressEvent = mouseButtonAction & mouseButtonStates;
	if (pressEvent)
	{
		auto pressEvent = mouseButtonAction & mouseButtonStates;
		for (auto obs : observers) {
			obs->OnMouseBtnPress(props.cursorPos.x, props.cursorPos.y, pressEvent, keyMods);
		}
	}

	// Mouse button release event
	auto releaseEvent = mouseButtonAction & (~mouseButtonStates);
	if (releaseEvent)
	{
		for (auto obs : observers) {
			obs->OnMouseBtnRelease(props.cursorPos.x, props.cursorPos.y, releaseEvent, keyMods);
		}
	}

	// Key events
	if (registeredKeyEvents)
	{
		for (int i = 0; i < registeredKeyEvents; i++) {
			for (auto obs : observers)
				keyStates[keyEvents[i]] ? obs->OnKeyPress(keyEvents[i], keyMods) : obs->OnKeyRelease(keyEvents[i], keyMods);
		}
		registeredKeyEvents = 0;
	}

	// Continuous events
	for (auto obs : observers) {
			obs->OnInputUpdate(static_cast<float>(deltaFrameTime), keyMods);
	}

	mouseButtonAction = 0;
}

void WindowObject::MakeCurrentContext() const
{
	// GLFW current context ?! Maybe because of HDC!
	glfwMakeContextCurrent(window);

	if (useNativeHandles)
	{
		#if defined(_WIN32) && !defined(OPENGL_ES)
			auto result = wglMakeCurrent(HDC(openglHandle), HGLRC(nativeRenderingContext));
			assert(result == TRUE);
		#endif
	}
	
	CheckOpenGLError();
}

void WindowObject::SetSize(int width, int height)
{
	glfwSetWindowSize(window, width, height);
	props.resolution = glm::ivec2(width, height);
	props.aspectRatio = float(width) / height;
	glViewport(0, 0, width, height);
}

glm::ivec2 WindowObject::GetResolution() const
{
	return props.resolution;
}

void WindowObject::UseNativeHandles(bool value)
{
	useNativeHandles = value;
}

void WindowObject::SwapBuffers() const
{
	glfwSwapBuffers(window);
	CheckOpenGLError();
}
