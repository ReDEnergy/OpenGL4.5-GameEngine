//#include <pch.h>
#include "EventListener.h"

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

EventListener::EventListener() {
}

EventListener::~EventListener() {
	Manager::GetEvent()->UnSubscribe(this);
}

void EventListener::SubscribeToEvent(const string & eventID)
{
	Manager::GetEvent()->Subscribe(this, eventID);
}

void EventListener::UnsubscribeFrom(const string & eventID)
{
	Manager::GetEvent()->UnSubscribe(this, eventID);
}

void EventListener::SubscribeToEvent(EventType Event)
{
	Manager::GetEvent()->Subscribe(this, Event);
}

void EventListener::UnsubscribeFrom(EventType Event)
{
	Manager::GetEvent()->UnSubscribe(this, Event);
}
