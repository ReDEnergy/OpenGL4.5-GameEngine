#pragma once

#include <list>
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>

#include <Core/Engine.h>
#include <Manager/Manager.h>
#include <Manager/EventSystem.h>
#include <Event/EventType.h>
#include <Event/EventListener.h>

#include <include/gl.h>

using namespace std;

template <class T>
class TimerEvent
{
	public:
		TimerEvent(T channel, float triggerInterval = 1, void *data = nullptr)
		{
			this->channel = channel;
			this->data = data;
			this->triggerInterval = triggerInterval;
			timePassed = 0;
		}

		float GetTriggerInterval() const
		{
			return triggerInterval;
		}

		const T& GetChannel() const
		{
			return channel;
		}

		void SetNewInterval(float interval)
		{
			this->triggerInterval = interval;
			Reset();
		}

		void SetNewEventChannel(T channel)
		{
			this->channel = channel;
			Reset();
		}

		void Reset() {
			timePassed = 0;
		}

		bool Update(float frameTime)
		{
			timePassed += frameTime;
			if (timePassed > triggerInterval) {
				timePassed = 0;
				Manager::Event->EmitAsync(channel, data);
				return true;
			}
			return false;
		}

	private:
		T channel;
		void *data;
		float timePassed;
		float triggerInterval;
};

template <class ChannelType>
class TimerManager
{
	friend class EventSystem;

	public:
		TimerManager() {};
		~TimerManager() {};

		TimerEvent<ChannelType>* Create(ChannelType Channel, float triggerInterval = 1.0f, void *data = nullptr)
		{
			auto event = new TimerEvent<ChannelType>(Channel, triggerInterval, data);
			return event;
		}

		void Add(TimerEvent<ChannelType>& event)
		{
			toAdd.push_back(&event);
		}

		void Remove(TimerEvent<ChannelType>& event)
		{
			toRemove.push_back(&event);
		}

	protected:
		void Update(float frameTime)
		{
			if (toRemove.size()) {
				for (auto event : toRemove) {
					events.remove(event);
				}
				toRemove.clear();
			}
			if (toAdd.size()) {
				for (auto event : toAdd) {
					event->Reset();
					events.push_back(event);
				}
				toAdd.clear();
			}

			for (auto event : events) {
				event->Update(frameTime);
			}
		}

	private:
		list<TimerEvent<ChannelType>*> toRemove;
		list<TimerEvent<ChannelType>*> toAdd;
		list<TimerEvent<ChannelType>*> events;
};