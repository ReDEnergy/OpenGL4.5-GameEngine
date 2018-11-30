#pragma once
#include <Event/EventListener.h>
#include <Component/Text.h>

#include <unordered_map>
#include <string>

namespace UIOverlay {
	class CommandBufferEntry : public Object {
		CommandBufferEntry() {};
		std::string ID;
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
		void Toggle(const std::string messageID, bool state = true);

		// Renders to the current framebuffer attached
		void Render() const;

		void CacheScores(int number);

	private:
		std::unordered_map<std::string, UIText> messages;
};

