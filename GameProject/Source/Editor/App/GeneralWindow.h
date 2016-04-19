#pragma once
#include <Editor/Windows/Interface/DockWindow.h>

class GeneralWindow
	: public DockWindow
{
	public:
		GeneralWindow();
		virtual ~GeneralWindow() {};

	private:
		void InitUI();
		void ToggleMaskChannel(int index);
};