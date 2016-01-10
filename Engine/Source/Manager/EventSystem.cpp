#include "EventSystem.h"

#include <include/gl.h>

#include <Core/Engine.h>
#include <Event/EventListener.h>
#include <Event/TimerEvent.h>
#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>

EventSystem::EventSystem() {
	Manager::Debug->InitManager("Event");
	dynamicTimers = new TimerManager<string>();
	standardTimers = new TimerManager<EventType>();
}

EventSystem::~EventSystem() {

}

void EventSystem::Subscribe(EventListener *E, const string & eventID) {
	 listeners[eventID].push_back(E);
}

void EventSystem::UnSubscribe(EventListener * E, const string & eventID)
{
	auto listenerList = listeners.find(eventID);
	if (listenerList != listeners.end()) {
		listenerList->second.remove(E);
	}
}

void EventSystem::Subscribe(EventListener *E, EventType Event) {
	listenersEnum[Event].push_back(E);
}

void EventSystem::UnSubscribe(EventListener * E, EventType Event)
{
	listenersEnum[Event].remove(E);
}

void EventSystem::EmitAsync(const string eventID, void *data) {
	auto list = listeners[eventID];
	for (auto listener: list) {
		eventQueueString.push_back(EventQueueElem<string>(listener, eventID, data));
	}
}

void EventSystem::EmitSync(const string eventID, void * data)
{
	auto list = listeners[eventID];
	for (auto listener : list) {
		listener->OnEvent(eventID, data);
	}
}

void EventSystem::EmitAsync(EventType Event, void *data) {
	auto list = listenersEnum[Event];
	for (auto listener: list) {
		eventQueueEnum.push_back(EventQueueElem<EventType>(listener, Event, data));
	}
}

void EventSystem::EmitSync(EventType Event, void * data)
{
	auto list = listenersEnum[Event];
	for (auto listener : list) {
		listener->OnEvent(Event, data);
	}
}

// TODO: hold subscription channels in the event object to speed up unsubscription
void EventSystem::UnSubscribe(EventListener * E)
{
	for (auto &L : listeners) {
		L.second.remove(E);
	}
	for (auto &L : listenersEnum) {
		L.remove(E);
	}
}

void EventSystem::Update()
{
	float frameTime = Engine::GetLastFrameTime();
	ProcessEvents();
	dynamicTimers->Update(frameTime);
	standardTimers->Update(frameTime);
}

void EventSystem::Clear() {
	listeners.clear();
	listenersEnum->clear();
}

TimerManager<string>* EventSystem::GetDynamicTimers() const
{
	return dynamicTimers;
}

TimerManager<EventType>* EventSystem::GetStandardTimers() const
{
	return standardTimers;
}

void EventSystem::ProcessEvents()
{
	{
		if (eventQueueEnum.size()) {
			auto toProcess = eventQueueEnum;
			eventQueueEnum.clear();
			for (auto &E : toProcess) {
				E.Process();
			};
		}
	}
	{
		if (eventQueueString.size()) {
			auto toProcess = eventQueueString;
			eventQueueString.clear();
			for (auto &E : toProcess) {
				E.Process();
			};
		}
	}
}