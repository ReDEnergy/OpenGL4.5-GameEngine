#pragma once
#include <string>
#include <list>

#include <include/dll_export.h>
#include <include/glm.h>
#include <include/gl_defines.h>

#ifdef OPENGL_ES
class GLESContext;
#endif

class ObjectInput;

class DLLExport WindowProperties
{
	public:
		struct GLContext
		{
			int major = 3;
			int minor = 3;
		};

	public:
		WindowProperties(bool shareContext = true);
		bool IsSharedContext() const;

	public:
		std::string name;
		glm::ivec2 resolution;
		glm::ivec2 position;
		glm::ivec2 cursorPos;
		float aspectRatio;
		bool resizable;
		bool visible;
		bool fullScreen;
		bool centered;
		bool hideOnClose;
		bool vSync;
		GLContext glContext;

	private:
		const bool sharedContext;
};

/*
 * Class WindowObject
 */

class DLLExport WindowObject
{
	friend class WindowManager;
	friend class InputSystem;
	friend class ObjectInput;

	public:
		WindowObject(WindowProperties properties);
		~WindowObject();

		void Show();
		void Hide();
		void Close();
		int ShouldClose() const;

		void ShowPointer();
		void CenterPointer();
		void SetPointerPosition(int mousePosX, int mousePosY);
		void HidePointer();
		void DisablePointer();

		void SetWindowPosition(glm::ivec2 position);
		void CenterWindow();

		void SwapBuffers() const;
		void SetVSync(bool state);
		bool ToggleVSync();

		void MakeCurrentContext() const;
		void UseNativeHandles(bool value);

		// Window Information
		void SetSize(int width, int height);
		glm::ivec2 GetResolution() const;

		// OpenGL State
		GLFWwindow* GetGLFWWindow() const;
	
		// Window Event
		void PollEvents() const;

		// Get Input State
		bool KeyHold(int keyCode) const;
		bool MouseHold(int button) const;
		int GetSpecialKeyState() const;

		// Event Dispatch - TODO - should be protected
		void UpdateObservers();

	protected:
		// Frame time
		void ComputeFrameTime();
		
		// Window Creation
		void InitWindow();
		void FullScreen();
		void WindowMode();

		// Subscribe to receive input events
		void Subscribe(ObjectInput * IC);

		// Input Processing
		void KeyCallback(int key, int scanCode, int action, int mods);
		void MouseButtonCallback(int button, int action, int mods);
		void MouseMove(int posX, int posY);
		void MouseScroll(double offsetX, double offsetY);

	private:
		void SetWindowCallbacks();

	public:
		WindowProperties props;
		GLFWwindow* window;

		// Native handles
		void *openglHandle;
		void *nativeRenderingContext;

	private:
		// Frame Time
		unsigned int frameID;
		double elapsedTime;
		double deltaFrameTime;

		bool useNativeHandles;

#ifdef OPENGL_ES
		GLESContext *eglContext;
#endif
		bool allowedControl;
		bool hiddenPointer;
		bool cursorClip;

		// Mouse button callback
		int mouseButtonCallback;			// Bit field for button callback
		int mouseButtonAction;				// Bit field for button state
		int mouseButtonStates;				// Bit field for mouse button state

		// Mouse move event
		bool mouseMoved;
		int mouseDeltaX;
		int mouseDeltaY;

		bool scrollEvent;
		double scrollOffsetX;
		double scrollOffsetY;

		// States for keyboard buttons - PRESSED(true) / RELEASED(false)
		int registeredKeyEvents;
		int keyEvents[128];
		bool keyStates[384];

		// Platform specific key codes - PRESSED(true) / RELEASED(false)
		bool keyScanCode[512];

		// Special keys (ALT, CTRL, SHIFT, CAPS LOOK, OS KEY) active alongside with normal key or mouse input
		int keyMods;	

		// Input Observers
		std::list<ObjectInput*> observers;
};