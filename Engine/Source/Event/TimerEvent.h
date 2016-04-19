#pragma once

#include <list>
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include <iostream>

#include <Core/Engine.h>
#include <Manager/Manager.h>
#include <Manager/EventSystem.h>
#include <Event/EventType.h>
#include <Event/EventListener.h>

template <class T>
class TimerEvent
{
	public:
		TimerEvent(T channel, float triggerInterval = 1, void *data = nullptr)
		{
			this->channel = channel;
			this->data = data;
			this->triggerInterval = triggerInterval;
			paused = false;
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

		void Pause() {
			paused = true;
		}

		void Resume() {
			paused = false;
		}

		bool Update(float frameTime)
		{
			if (paused) return false;
			timePassed += frameTime;
			if (timePassed > triggerInterval) {
				timePassed = 0;
				Manager::GetEvent()->EmitAsync(channel, data);
				for (auto &listener : listeners) {
					listener();
				}
				return true;
			}
			return false;
		}

		void OnTrigger(std::function<void()> onUpdate) {
			listeners.push_back(onUpdate);
		}

	private:
		T channel;
		void *data;
		float paused;
		float timePassed;
		float triggerInterval;
		std::list<std::function<void()>> listeners;
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
		std::list<TimerEvent<ChannelType>*> toRemove;
		std::list<TimerEvent<ChannelType>*> toAdd;
		std::list<TimerEvent<ChannelType>*> events;
};
