//#include <pch.h>
#include "EventListener.h"

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

EventListener::EventListener() {
}

EventListener::~EventListener() {
}

void EventListener::SubscribeToEvent(string eventID) {
	Manager::Event->Subscribe(this, eventID);
}

void EventListener::SubscribeToEvent(EventType Event) {
	Manager::Event->Subscribe(Event, this);
}
