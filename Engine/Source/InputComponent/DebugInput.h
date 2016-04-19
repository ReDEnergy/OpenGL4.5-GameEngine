#pragma once
#include <include/dll_export.h>

#include <Component/ObjectInput.h>
#include <Event/EventListener.h>

class DLLExport DebugInput
	: public ObjectInput,
	public EventListener
{
	public:
		DebugInput();
		void OnKeyPress(int key, int mods);
};

