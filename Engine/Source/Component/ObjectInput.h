#pragma once
#include <Core/Object.h>
#include <include/utils.h>

class WindowObject;

class DLLExport ObjectInput : virtual public Object
{
	friend class WindowObject;

	public:
		ObjectInput();
		virtual ~ObjectInput();

		void AttachTo(const char* windowName);
		void AttachTo(WindowObject* windowName);

	protected:
		// Method will be called each frame before the Update() method
		// Use for real-time interaction with the OpenGL window such as performing continuous updates when pressing a key
		// @param deltaTime - frame time in seconds for the previous frame, may be used for frame time independent updates
		// @param mods - bit combination of GLFW_MOD_"KEY" states: CONTROL, ALT, SHIFT, SUPER
		virtual void OnInputUpdate(float deltaTime, int mods) {};

		// Method will be called each frame before the Update() method
		// @param key - GLFW_KEY_"KEYCODE", where KEYCODE is a standard keyboard key
		// @param mods - bit combination of GLFW_MOD_* states: CONTROL, ALT, SHIFT, SUPER
		virtual void OnKeyPress(int key, int mods) {};

		// Method will be called each frame before the Update() method
		// @param key - GLFW_KEY_"KEYCODE", where KEYCODE is a standard keyboard key
		// @param mods - bit combination of GLFW_MOD_* states: CONTROL, ALT, SHIFT, SUPER
		virtual void OnKeyRelease(int key, int mods) {};

		// Method will be called each frame before the Update() method
		// @param mouseX - X coordinate of the mouse position relative to the top-left corner ([0, 0])
		// @param mouseY - Y coordinate of the mouse position relative to the top-left corner
		// @param deltaX - offsetX in pixels from the last reported position
		// @param deltaY - offsetY in pixels from the last reported position
		virtual void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {};

		// Method will be called each frame before the Update() method
		// @param mouseX - X coordinate of the mouse position relative to the top-left corner ([0, 0])
		// @param mouseY - Y coordinate of the mouse position relative to the top-left corner
		// @param button - bit-mask with the buttons currently pressed. Use the preprocessor helpers for testing:
		//				    
		//						IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)
		//						IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT)
		//						IS_BIT_SET(button, GLFW_MOUSE_BUTTON_*) ... etc
		//
		// @param mods   - bit combination of GLFW_MOD_* states: CONTROL, ALT, SHIFT, SUPER
		virtual void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {};

		// Same as OnMouseBtnPress but triggered when a button is released
		virtual void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {};

	protected:
		WindowObject *window;
};