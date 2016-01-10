#pragma once
#include <Core/Object.h>
#include <Input/InputRules.h>

class DLLExport ObjectInput : virtual public Object
{
	public:
		ObjectInput(InputGroup GROUP = InputGroup::IG_GAMEPLAY);
		virtual ~ObjectInput();
		virtual void OnInputUpdate(float deltaTime, int mods) {};
		virtual void OnKeyPress(int key, int mods) {};
		virtual void OnKeyRelease(int key, int mods) {};
		virtual void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {};
		virtual void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods) {};
		virtual bool IsActive();

	protected:
		InputGroup GROUP;
};

