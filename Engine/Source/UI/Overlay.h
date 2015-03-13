#pragma once
#include <Event/EventListener.h>
#include <Component/Text.h>
#include <vector>

class DLLExport Overlay : public EventListener {
	public:
		Overlay();
		~Overlay();

		void Update(float deltaTime);
		void OnEvent(EventType Event, Object *data);
		void CacheScores(int number);

	// TODO
	private:
		vector<Text*> messages;
		Text *message;
		float messageTime;
		char* name;
};

