#pragma once
#include <Core/Object.h>
#include <Input/InputRules.h>

class DLLExport ObjectInput : virtual public Object {
	public:
		ObjectInput(InputGroup GROUP);
		virtual ~ObjectInput();
		virtual void Update(float deltaTime, int mods) {};
		virtual void OnKeyPress(int key, int mods) {};
		virtual void OnKeyRelease(int key, int mods) {};
		virtual void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {};
		virtual bool IsActive();

	private:
		InputGroup GROUP;
};

