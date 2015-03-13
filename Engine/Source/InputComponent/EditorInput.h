#pragma once
#include <Core/InputSystem.h>
#include <Component/ObjectInput.h>

using namespace std;

class EditorInput : public ObjectInput {
	public:
		EditorInput();
		void OnKeyPress(int key, int mods);
};

