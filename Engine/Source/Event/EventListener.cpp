//#include <pch.h>
#include "EventListener.h"

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

EventListener::EventListener() {
}

EventListener::~EventListener() {
}

void EventListener::SubscribeToEvent(const string & eventID)
{
	Manager::GetEvent()->Subscribe(this, eventID);
}

void EventListener::SubscribeToEvent(EventType Event) {
	Manager::GetEvent()->Subscribe(Event, this);
}
