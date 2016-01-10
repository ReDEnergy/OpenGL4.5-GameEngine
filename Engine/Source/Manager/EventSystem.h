#pragma once

#include <list>
#include <unordered_map>
#include <string>
#include <vector>

#include <Event/EventType.h>
#include <Event/TimerEvent.h>
#include <Event/EventListener.h>
#include <Event/SimpleTimer.h>

using namespace std;


template <class T>
class EventQueueElem
{
	public:
		EventQueueElem(EventListener *E, T channel, void *data)
			: listener(E), channel(channel), data(data)
		{};

		void Process() {
			listener->OnEvent(channel, data);
		};

	private:
		EventListener *listener;
		T channel;
		void *data;
};

class DLLExport EventSystem
{
	protected:
		EventSystem();
		~EventSystem();

	public:
		void Subscribe(EventListener *E, const string& eventID);
		void UnSubscribe(EventListener *E, const string& eventID);
		void EmitAsync(const string eventID, void *data = nullptr);
		void EmitSync(const string eventID, void *data = nullptr);

		void Subscribe(EventListener *E, EventType Event);
		void UnSubscribe(EventListener *E, EventType Event);
		void EmitAsync(EventType Event, void *data = nullptr);
		void EmitSync(EventType Event, void *data = nullptr);

		void UnSubscribe(EventListener *E);

		void Update();
		void Clear();

		// Timer Events
		TimerManager<string>* GetDynamicTimers() const;
		TimerManager<EventType>* GetStandardTimers() const;

	private:
		void ProcessEvents();

	public:
		TimerManager<string> *dynamicTimers;
		TimerManager<EventType> *standardTimers;

	private:
		unordered_map<string, list<EventListener*>> listeners;
		list<EventListener*> listenersEnum[EventType::SIZE];

		vector<EventQueueElem<EventType>> eventQueueEnum;
		vector<EventQueueElem<string>> eventQueueString;
};