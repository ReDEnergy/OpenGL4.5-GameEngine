#pragma once

#include <Core/InputSystem.h>
#include <Component/ObjectInput.h>
#include <Event/EventListener.h>

class DLLExport EditorInput :
	public ObjectInput,
	public EventListener
{
	public:
		EditorInput();
		void OnKeyPress(int key, int mods);
		void OnEvent(const std::string& eventID, void * data);
};

