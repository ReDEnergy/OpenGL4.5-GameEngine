#pragma once
#include <list>

#include <include/dll_export.h>
#include <include/gl.h>

using namespace std;

class ObjectInput;

namespace SPECIAL_KEY {
	enum _SPECIAL_KEY {
		SHIFT = 1,
		CTRL = 2,
		ALT = 4,
		OS_KEY = 8
	};
}

namespace MOUSE_BUTTON {
	enum _MOUSE_BUTTON {
		LEFT	= 0,
		RIGHT	= 1,
		MIDDLE	= 2,
		FORWARD	= 3,
		BACK	= 4,
	};
}


class DLLExport InputSystem
{
	private:
		InputSystem();

	public:
		// KeyBoard
		static bool KeyHold(int keyCode);
		static bool MouseHold(int button);

		static void KeyCallback(GLFWwindow *W, int key, int scancode, int action, int mods);
		static void Init();

		static int GetMods();

		// Mouse
		static void CursorMove(GLFWwindow *W, double posX, double posY);
		static void MouseClick(GLFWwindow *W, int button, int action, int mods);

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
		static bool *mouseStates;
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