#pragma once
#include <Editor/Windows/Module/ModuleWindow.h>

class SimpleCheckBox;

class GeneralWindow : public ModuleWindow<int>
{
	public:
		GeneralWindow();
		virtual ~GeneralWindow() {};

	private:
		void InitUI();
		void ToggleMaskChannel(int index);

	private:
		SimpleCheckBox *logUnsafeEvents;
};