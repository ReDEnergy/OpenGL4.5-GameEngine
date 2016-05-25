#include "WindowObject.h"
#include "WindowObject.h"
#include "WindowObject.h"

#include <iostream>
#include <include/gl.h>
#include <include/glm_utils.h>

#include <Core/Engine.h>
#include <Core/InputSystem.h>
#include <Core/WindowManager.h>
#include <Core/GLES/GLESContext.h>
#include <Component/ObjectInput.h>

#include <include/gl_native.h>

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
	useNativeHandles = false;
	props.aspectRatio = float(props.resolution.x) / props.resolution.y;

	// Init OpenGL Window
	props.fullScreen ? FullScreen() : WindowMode();

	CheckOpenGLError();

	// Set default state
	memset(mouseStates, 0, 3);
	memset(keyStates, 0, 384);
	memset(keyScanCode, 0, 512);

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
	wglSwapIntervalEXT(state);
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
	#ifdef OPENGL_ES
	if (eglContext) return 0;
	#endif
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
	auto currentTime = glfwGetTime();
	deltaFrameTime = float(currentTime - elapsedTime);
	elapsedTime = currentTime;
}

void WindowObject::FullScreen()
{
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *videoDisplay = glfwGetVideoMode(monitor);
	window = glfwCreateWindow(videoDisplay->width, videoDisplay->height, props.name.c_str(), monitor, NULL);
	glfwMakeContextCurrent(window);
	SetSize(videoDisplay->width, videoDisplay->height);
	SetWindowCallbacks();
}

void WindowObject::WindowMode()
{
	glfwWindowHint(GLFW_VISIBLE, props.visible);

	#ifdef OPENGL_ES
		eglContext = new GLESContext();
		eglContext->Init();
		if (eglContext->GetContext()) return;

		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	#else
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#endif

	GLFWwindow *shaderCtxWindow = NULL;
	if (props.IsSharedContext()) {
		auto W = WindowManager::GetShaderdWindowContext();
		if (W) shaderCtxWindow = W->GetGLFWWindow();
	}

	window = nullptr;
	window = glfwCreateWindow(props.resolution.x, props.resolution.y, props.name.c_str(), NULL, shaderCtxWindow);
	assert(window != nullptr);
	glfwMakeContextCurrent(window);

	if (props.centered) {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *videoDisplay = glfwGetVideoMode(monitor);
		auto screenSize = glm::ivec2(videoDisplay->width, videoDisplay->height);
		props.position = (screenSize - props.resolution) / 2;
	}

	#ifdef _WIN32
		openglHandle = wglGetCurrentDC();
		nativeRenderingContext = glfwGetWGLContext(window);
	#endif

	glfwSetWindowPos(window, props.position.x, props.position.y);
	SetSize(props.resolution.x, props.resolution.y);
	SetWindowCallbacks();
}

void WindowObject::SetWindowCallbacks()
{
	glfwSetWindowCloseCallback(window, WindowManager::OnClose);
	glfwSetWindowSizeCallback(window, WindowManager::OnResize);
	glfwSetKeyCallback(window, InputSystem::KeyCallback);
	glfwSetMouseButtonCallback(window, InputSystem::MouseClick);
	glfwSetCursorPosCallback(window, InputSystem::CursorMove);
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
	return mouseStates[button];
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
	keyStates[key] = action ? true : false;
	keyMods = mods;

	if (action == GLFW_PRESS) {
		for (auto obs : observers)
				obs->OnKeyPress(key, mods);
		return;
	}
	if (action == GLFW_RELEASE) {
		for (auto obs : observers)
			obs->OnKeyRelease(key, mods);
	}
}

void WindowObject::MouseButtonCallback(int button, int action, int mods)
{
	// TODO treat input
	keyMods = mods;
	mouseStates[button] = action ? true : false;
	for (auto obs : observers) {
		obs->OnMouseBtnEvent(props.cursorPos.x, props.cursorPos.y, button, action, mods);
	}
}

void WindowObject::MouseMove(int posX, int posY)
{
	int deltaX = posX - props.cursorPos.x;
	int deltaY = posY - props.cursorPos.y;
	props.cursorPos = glm::ivec2(posX, posY);

	for (auto obs : observers) {
		obs->OnMouseMove(props.cursorPos.x, props.cursorPos.y, deltaX, deltaY);
	}
}

void WindowObject::UpdateObserver()
{
	ComputeFrameTime();
	for (auto obs : observers) {
			obs->OnInputUpdate(deltaFrameTime, keyMods);
	}
}

void WindowObject::MakeCurrentContext() const
{
	// glfw current context ?! Maybe because of HDC!
	glfwMakeContextCurrent(window);

	#ifdef OPENGL_ES
		eglContext->MakeCurrent();
	#endif

	if (useNativeHandles)
	{
		#if defined(_WIN32)
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
	#ifdef OPENGL_ES
		eglContext->SwapBuffer();
	#else
		glfwSwapBuffers(window);
	#endif

	CheckOpenGLError();
}
