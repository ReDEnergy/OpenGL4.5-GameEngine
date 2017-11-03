#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

class EventChannel
{
	public:
		EventChannel(std::string name);
		~EventChannel();

		void EmitAsync(const std::string& eventID, void *data);
		void EmitSync(const std::string& eventID, void *data);
		void Subscribe(const std::string& eventID, std::function<void(void*)> callback);
		void ProcessEvents();

		#ifdef _DEBUG
		void LogUnsafeEvents(bool value);
		#endif

	private:

		// Callback class with thread safe check on _DEBUG
		class Callback
		{
			public:
				Callback(std::function<void(void*)> callback)
				{
					this->callback = callback;
					#ifdef _DEBUG
					threadID = std::this_thread::get_id();
					#endif
				}

				inline void Call(void *data) {
					callback(data);
				}

			private:
				std::function<void(void*)> callback;

			public:
				#ifdef _DEBUG
				std::thread::id threadID;
				#endif
		};

		// Event listener
		class Listener
		{
			public:
				void AddCallback(const Callback &callback);

				std::vector<Callback> callbacks;
				std::vector<void*> asyncData;
				std::mutex _mutex;
		};

		std::string name;
		std::unordered_map<std::string, Listener> listeners;

		// thread safe synchronization mechanism
		std::mutex _mutex;
		std::mutex _mutexWrite;
		std::condition_variable cv;
		std::atomic<int> readers;

		#ifdef _DEBUG
		bool logUnsafeEvents;
		std::thread::id threadID;
		#endif
};
