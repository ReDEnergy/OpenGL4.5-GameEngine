#include "EventSystem.h"

#include <Core/Engine.h>
#include <Event/EventListener.h>
#include <Event/TimerEvent.h>
#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>

using uint = unsigned int;
using namespace std;

EventSystem::EventSystem()
{
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
	listenersEnum[static_cast<uint>(Event)].push_back(E);
}

void EventSystem::Subscribe(EventType Event, std::function<void(void*)> listener)
{
	funcEnumListeners[static_cast<uint>(Event)].push_back(listener);
}

void EventSystem::UnSubscribe(EventListener * E, EventType Event)
{
	listenersEnum[static_cast<uint>(Event)].remove(E);
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
	auto list = listenersEnum[static_cast<uint>(Event)];
	for (auto listener: list) {
		eventQueueEnum.push_back(EventQueueElem<EventType>(listener, Event, data));
	}
}

void EventSystem::EmitSync(EventType Event, void * data)
{
	auto list = listenersEnum[static_cast<uint>(Event)];
	for (auto listener : list) {
		listener->OnEvent(Event, data);
	}

	auto obs = funcEnumListeners[static_cast<uint>(Event)];
	for (auto func : obs) {
		func(data);
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

void EventSystem::Subscribe(const char* channelID, const string & eventID, function<void(void*)> listener)
{
	channelListeners[channelID][eventID].funcListeners.push_back(listener);
}

void EventSystem::EmitAsync(const char * channelID, const string & eventID, void * data)
{
	channelData[channelID][eventID].push_back(data);
}

void EventSystem::EmitSync(const char * channelID, const string & eventID, void * data)
{
	auto it = channelListeners.find(channelID);
	if (it != channelListeners.end()) {
		for (auto &func : it->second[eventID].funcListeners) {
			func(data);
		}
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
	if (eventQueueEnum.size()) {
		auto toProcess = eventQueueEnum;
		eventQueueEnum.clear();
		for (auto &E : toProcess) {
			E.Process();
		};
	}

	if (eventQueueString.size()) {
		auto toProcess = eventQueueString;
		eventQueueString.clear();
		for (auto &E : toProcess) {
			E.Process();
		};
	}
}

void EventSystem::ProcessChannelEvents(const char * channelID)
{
	auto it = channelData.find(channelID);
	if (it != channelData.end()) {
		for (auto &lists : it->second) {
			for (auto &data : it->second[lists.first]) {
				for (auto &listener : channelListeners[channelID][lists.first].funcListeners) {
					listener(data);
				}
			}
			channelData[channelID][lists.first].clear();
		}
	}
}
