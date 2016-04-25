#pragma once
#include <Core/Object.h>

class WindowObject;

class DLLExport ObjectInput : virtual public Object
{
	friend class WindowObject;

	public:
		ObjectInput();
		virtual ~ObjectInput();

		void AttachTo(const char* windowName);
		void AttachTo(WindowObject* windowName);
		virtual bool IsActive();

	protected:
		virtual void OnInputUpdate(float deltaTime, int mods) {};
		virtual void OnKeyPress(int key, int mods) {};
		virtual void OnKeyRelease(int key, int mods) {};
		virtual void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {};
		virtual void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods) {};

	protected:
		WindowObject *window;
};

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
		LEFT = 0,
		RIGHT = 1,
		MIDDLE = 2,
		FORWARD = 3,
		BACK = 4,
	};
}