#pragma once

#include <list>
#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <mutex>
#include <thread>

#include <Event/EventType.h>
#include <Event/EventListener.h>

class EventChannel;

template <class T>
class TimerManager;

template <class T>
class EventQueueElem
{
	public:
		EventQueueElem(EventListener *E, T channel, void *data)
			: listener(E), channel(channel), data(data)
		{};

		void Process() {
			listener->OnEvent(channel, data);
		};

	private:
		EventListener *listener;
		T channel;
		void *data;
};


class DLLExport EventSystem
{
	protected:
		EventSystem();
		~EventSystem();

	public:
		void Subscribe(EventListener *E, const std::string& eventID);
		void UnSubscribe(EventListener *E, const std::string& eventID);
		void EmitAsync(const std::string eventID, void *data = nullptr);
		void EmitSync(const std::string eventID, void *data = nullptr);

		void Subscribe(EventListener *E, EventType Event);
		void Subscribe(EventType Event, std::function<void(void*)> listener);
		void UnSubscribe(EventListener *E, EventType Event);
		void EmitAsync(EventType Event, void *data = nullptr);
		void EmitSync(EventType Event, void *data = nullptr);

		void UnSubscribe(EventListener *E);

		// Support for dynamic channels with direct messaging
		void LogUnsafeEvents(bool value);
		void ProcessChannelEventsInMainUpdate(const char* channelID, bool value);
		void Subscribe(const char* channelID, const std::string& eventID, std::function<void(void*)> listener);
		void EmitAsync(const char* channelID, const std::string& eventID, void *data);
		void EmitSync(const char* channelID, const std::string& eventID, void *data);
		void ProcessChannelEvents(const char* channelID);

		void Update();
		void Clear();

		// Timer Events
		TimerManager<std::string>* GetDynamicTimers() const;
		TimerManager<EventType>* GetStandardTimers() const;

	private:
		void ProcessEvents();

	public:
		TimerManager<std::string> *dynamicTimers;
		TimerManager<EventType> *standardTimers;

	private:

		bool logUnsafeEvents;

		// thread safe synchronization mechanism
		std::mutex _mutex_channels;

		//struct ChannelListeners {
		//	std::vector < std::function<void(void*)>> funcListeners;
		//};
		// Channel based events
		//std::unordered_map<std::string, std::unordered_map<std::string, ChannelListeners>> channelListeners;
		//std::unordered_map<std::string, std::unordered_map<std::string, std::vector<void*>>> channelData;

		std::list<std::string> channelProcessingList;
		std::unordered_map<std::string, EventChannel*> channels;

		std::unordered_map<std::string, std::list<EventListener*>> listeners;
		std::list<EventListener*> listenersEnum[static_cast<unsigned int>(EventType::SIZE)];
		std::list<std::function<void(void*)>> funcEnumListeners[static_cast<unsigned int>(EventType::SIZE)];

		std::vector<EventQueueElem<EventType>> eventQueueEnum;
		std::vector<EventQueueElem<std::string>> eventQueueString;
};