#pragma once

#include <list>
#include <unordered_map>
#include <string>

#include <Event/EventType.h>
#include <Event/EventListener.h>

using namespace std;

class DLLExport EventSystem
{
	protected:
		EventSystem();
		~EventSystem();

	public:
		void Subscribe(EventListener *E, string eventID);
		void EmitSync(string eventID, Object *data);

		void Subscribe(EventType Event, EventListener *E);
		void EmitSync(EventType Event, Object *data);

		void Clear();

	private:
		unordered_map<string, list<EventListener*>> listeners;
		list<EventListener*> listenersEnum[EventType::SIZE];
};