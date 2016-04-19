#pragma once
#include <Editor/Windows/Interface/DockWindow.h>
#include <Editor/Interface/QtInput.h>

#include <include/glm.h>
#include <Event/EventListener.h>

class GameObject;
class ProfileTimer;
class SimpleTextBox;

class ProfilingWindow
	: public DockWindow
	, public EventListener
{
	public:
		ProfilingWindow();
		virtual ~ProfilingWindow() {};

	private:
		void InitUI();
		void UpdateUI();
		void Update();
		void OnEvent(EventType Event, void *data);
		void OnEvent(const string& eventID, void *data);

	private:
		ProfileTimer *profileInfo;
		vector<SimpleFloatInput*> channels;
};