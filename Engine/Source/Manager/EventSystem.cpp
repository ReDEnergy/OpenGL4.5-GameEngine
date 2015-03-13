//#include <pch.h>
#include "EventSystem.h"

#include <Manager/DebugInfo.h>
#include <Event/EventListener.h>
#include <Manager/Manager.h>

EventSystem::EventSystem() {
	Manager::Debug->InitManager("Event");
}

EventSystem::~EventSystem() {

}

void EventSystem::Subscribe(EventListener *E, const string eventID) {
	 listeners[eventID].push_back(E);
}

void EventSystem::Subscribe(EventType Event, EventListener *E) {
	listenersEnum[Event].push_back(E);
}

void EventSystem::EmitSync(const string eventID, Object *data) {
	auto list = listeners[eventID];
	for (auto listener: list) {
		listener->OnEvent(eventID.c_str(), data);
	}
}

void EventSystem::EmitSync(EventType Event, Object *data) {
	auto list = listenersEnum[Event];
	for (auto listener: list) {
		listener->OnEvent(Event, data);
	}
}

void EventSystem::Clear() {
	listeners.clear();
	listenersEnum->clear();
}