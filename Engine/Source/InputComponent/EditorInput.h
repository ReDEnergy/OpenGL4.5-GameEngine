#pragma once

#include <Core/InputSystem.h>
#include <Component/ObjectInput.h>
#include <Event/EventListener.h>

using namespace std;

class DLLExport EditorInput :
	public ObjectInput,
	public EventListener
{
	public:
		EditorInput();
		void OnKeyPress(int key, int mods);
		void OnEvent(const char * eventID, Object * data);
};

