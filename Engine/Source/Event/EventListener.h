#pragma once
#include <string>
#include <include/dll_export.h>

#include <Core/Object.h>
#include <Event/EventType.h>

using namespace std;

class DLLExport EventListener : virtual public Object {
	public:
		EventListener();
		virtual ~EventListener();

		virtual void OnEvent(const char* eventID, Object *data) {};
		void SubscribeToEvent(string eventID);

		virtual void OnEvent(EventType Event, Object *data) {};
		void SubscribeToEvent(EventType Event);
};

