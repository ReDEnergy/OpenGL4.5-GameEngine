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

		void SwapBuffers() const;
		void SetVSync(bool state);
		bool ToggleVSync();

		void MakeCurrentContext() const;
		void UseNativeHandles(bool value);

		// Window Information
		void SetSize(int width, int height);
		glm::ivec2 GetResolution() const;

		// OpenGL State
		bool IsCoreContext() const;
		GLFWwindow* GetGLFWWindow() const;
	
		// Window Event
		void PollEvents() const;

		// Get Input State
		bool KeyHold(int keyCode) const;
		bool MouseHold(int button) const;
		int GetSpecialKeyState() const;

		// Event Dispatch - TODO - should be protected
		void UpdateObserver();

	protected:
		// Frame time
		void ComputeFrameTime();
		
		// Window Creation
		void FullScreen();
		void WindowMode();

		// Subscribe to receive input events
		void Subscribe(ObjectInput * IC);

		// Input Processing
		void KeyCallback(int key, int scanCode, int action, int mods);
		void MouseButtonCallback(int button, int action, int mods);
		void MouseMove(int posX, int posY);

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
		double elapsedTime;
		float deltaFrameTime;

		bool useNativeHandles;

#ifdef OPENGL_ES
		GLESContext *eglContext;
#endif
		bool allowedControl;
		bool hiddenPointer;
		bool cursorClip;

		// States for keyboard buttons - PRESSED(true) / RELEASED(false)
		bool keyStates[384];

		// States for each mouse button - PRESSED(true) / RELEASED(false)
		bool mouseStates[3];

		// Platform specific key codes - PRESSED(true) / RELEASED(false)
		bool keyScanCode[512];

		// Special keys (ALT, CTRL, SHIFT, CAPS LOOK, OS KEY) active alogside with normal key or mouse input
		int keyMods;	

		// Input Observers
		std::list<ObjectInput*> observers;
};