#pragma once
#include <string>
#include <include/dll_export.h>

#include <Core/Object.h>
#include <Event/EventType.h>

class DLLExport EventListener :
	virtual public Object
{
	friend class EventSystem;

	public:
		EventListener();
		virtual ~EventListener();

		virtual void OnEvent(const std::string& eventID, void *data) {};
		void SubscribeToEvent(const std::string& eventID);
		void UnsubscribeFrom(const std::string& eventID);

		virtual void OnEvent(EventType Event, void *data) {};
		void SubscribeToEvent(EventType Event);
		void UnsubscribeFrom(EventType Event);
};

