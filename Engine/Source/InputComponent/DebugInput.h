#pragma once
#include <include/dll_export.h>

#include <Component/ObjectInput.h>
#include <Event/EventListener.h>

using namespace std;

class DLLExport DebugInput
	: public ObjectInput,
	public EventListener
{
	public:
		DebugInput();
		void OnKeyPress(int key, int mods);
		void OnEvent(const char* eventID, Object *data);
};

