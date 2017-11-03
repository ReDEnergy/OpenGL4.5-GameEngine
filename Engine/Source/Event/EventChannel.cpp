#include "EventChannel.h"

#include <Threading/utils.h>
#include <iostream>

using namespace std;

EventChannel::EventChannel(std::string name)
{
	#ifdef _DEBUG
	logUnsafeEvents = false;
	threadID = std::this_thread::get_id();
	#endif
	this->name = std::move(name);
}

EventChannel::~EventChannel()
{
}

void EventChannel::EmitAsync(const std::string & eventID, void * data)
{
	auto it = listeners.find(eventID);

	if (it != listeners.end()) {
		MUTEX_RAII_LOCK(it->second._mutex);
		it->second.asyncData.push_back(data);
	}
	#ifdef _DEBUG
	else {
		if (logUnsafeEvents)
			cout << "Channel [" << name << "] has no listeners for [" << eventID << "] event" << endl;
	}
	#endif
}

void EventChannel::EmitSync(const std::string & eventID, void * data)
{
	#ifdef _DEBUG
		auto thID = std::this_thread::get_id();
	#endif

	auto it = listeners.find(eventID);

	if (it != listeners.end()) {
		for (auto &callback : it->second.callbacks) {
			#ifdef _DEBUG
			if (logUnsafeEvents) {
				if (callback.threadID != thID) {
					cout << "[WARNING] [EventSystem] [" << name << "] Unsafe inter-thread communication: event [" << eventID << "]" << endl;
				}
			}
			#endif
			callback.Call(data);
		}
	}
	#ifdef _DEBUG
	else {
		if (logUnsafeEvents)
			cout << "Channel [" << name << "] has no listeners for [" << eventID << "] event" << endl;
	}
	#endif
}

void EventChannel::Subscribe(const std::string & eventID, std::function<void(void*)> callback)
{
	auto &l = listeners[eventID];
	l.AddCallback(Callback(callback));
}

void EventChannel::ProcessEvents()
{
	for (auto &listener : listeners) {
		MUTEX_RAII_LOCK(listener.second._mutex);
		for (auto &data : listener.second.asyncData) {
			for (auto &callback : listener.second.callbacks) {
				callback.Call(data);
			}
		}
		listener.second.asyncData.clear();
	}
}

#ifdef _DEBUG
void EventChannel::LogUnsafeEvents(bool value)
{
	logUnsafeEvents = value;
}
#endif

void EventChannel::Listener::AddCallback(const Callback &callback)
{
	MUTEX_RAII_LOCK(_mutex);
	callbacks.push_back(callback);
}
