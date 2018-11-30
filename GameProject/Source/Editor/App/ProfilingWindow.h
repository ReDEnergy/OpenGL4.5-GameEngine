#pragma once
#include <mutex>
#include <Editor/Windows/Module/ModuleWindow.h>

class GameObject;
class ProfilingTimer;
class SimpleTextBox;
class SimpleFloatInput;
class CustomWidget;

class ProfilingWindow : public ModuleWindow<int>
{
	public:
		ProfilingWindow();
		virtual ~ProfilingWindow() {};

	private:
		void InitUI();
		void UpdateList();
		void UpdateUI();
		void OnEvent(EventType Event, void *data);
		void OnEvent(const std::string& eventID, void *data);

	private:
		char textBoxBuffer[100];
		ProfilingTimer *appPerfInfo;
		ProfilingTimer *profileInfo;
		SimpleTextBox *frameInfo;
		SimpleFloatInput *frameRate;
		SimpleFloatInput *frameTime;
		CustomWidget *profilingArea;
		CustomWidget *frameInfoArea;
		std::atomic<bool> newPerfInfo;
		std::vector<SimpleFloatInput*> channels;
		std::list<SimpleFloatInput*> idleNodes;
		std::mutex copyPerfInfo;
};