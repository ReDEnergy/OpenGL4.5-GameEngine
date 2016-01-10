#pragma once
#include <Event/EventListener.h>
#include <Component/Text.h>

#include <unordered_map>
#include <string>

namespace UIOverlay {
	class Command : public Object {
		Command() {};
		string ID;
	};
}

class DLLExport DebugOverlayText //: public EventListener
{
	struct UIText {
		UIText() { 
			isActive = true;
		};
		UIText(Text *text, bool state)
			: text(text), isActive(state) {};
		Text *text;
		bool isActive;
	};

	public:
		DebugOverlayText();
		~DebugOverlayText();

		void Init();
		Text* Add(const char *textID, const char *value = NULL);
		void Toggle(const string messageID, bool state = true);
		void Update();
		void CacheScores(int number);

	private:
		unordered_map<string, UIText> messages;
};

