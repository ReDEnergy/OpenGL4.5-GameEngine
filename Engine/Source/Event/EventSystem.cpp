//#include <pch.h>
#include "EventSystem.h"

#include <include/gl.h>

#include <Manager/DebugInfo.h>
#include <Event/EventListener.h>
#include <Manager/Manager.h>

EventSystem::EventSystem() {
	Manager::Debug->InitManager("Event");
}

EventSystem::~EventSystem() {

}

void EventSystem::Subscribe(EventListener *E, const string &eventID) {
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

void EventSystem::Update()
{
	auto it = timedEvents.begin();
	while (it != std::end(timedEvents)) {
		if ((*it)->Update()) {
			it = timedEvents.erase(it);
		}
		else {
			it++;
		}
	}
}

void EventSystem::TriggerEvent(EventListener *E, EventType Event, Object *data, float delaySeconds)
{
	timedEvents.push_back(new TimedEvent(E, Event, data, delaySeconds));
}

void EventSystem::Clear() {
	listeners.clear();
	listenersEnum->clear();
}

TimedEvent::TimedEvent(EventListener *E, EventType Event, Object *data, float delaySeconds)
{
	event = E;
	type = Event;
	this->data = data;
	triggerTime = float(glfwGetTime()) + delaySeconds;
}

bool TimedEvent::Update()
{
	float deltaTime = float(glfwGetTime());
	if (float(glfwGetTime()) > triggerTime) {
		event->OnEvent(type, NULL);
		return true;
	}
	return false;
}
