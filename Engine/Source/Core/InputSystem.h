#pragma once
#include <list>

#include <include/dll_export.h>
#include <include/gl.h>

using namespace std;

class ObjectInput;

class DLLExport InputSystem
{
	private:
		InputSystem();

	public:
		// KeyBoard
		static bool KeyPressed(int keyCode);
		static bool KeyReleased(int keyCode);
		static bool KeyHold(int keyCode);
		static void KeyCallback(GLFWwindow *W, int key, int scancode, int action, int mods);
		static void ResetKeys();

		// Mouse
		static void CursorMove(GLFWwindow *W, double posX, double posY);

		// Event Dispatch
		static void Subscribe(ObjectInput* O);
		static void RemoveSubscription(ObjectInput* O);
		static void UpdateObservers(float deltaTime);

		// InputRule
		static void RuleUpdate();
		static void EndFrame();

	private:
		static void NotifyObservers(int key, int action, int mods);
		static void NotifyObservers(int mouseX, int mouseY, int deltaX, int deltaY);

	private:
		// KeyBuffering arrays for holding key state
		static bool *keyStates;
		static int scancode;
		static int mods;
		static bool ruleChanged;
		static list<ObjectInput*> observers;
		static list<ObjectInput*> activeObserver;
};

// TODO
// Strong relation bettwen this and WindowManager
// Inspect posibility to merge the classes
// Curent issues:
//	- input event should be stored per WindowObject
// Solutions:
//	- idetify input based on the active WindowObject
//	- activeWindow property
//	- PROBLEM
//		- can't hold the state between window switching
//	- merge the classes and each Window will have it's own InputSystem