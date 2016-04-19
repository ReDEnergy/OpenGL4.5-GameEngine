#pragma once
#include <Editor/Windows/Interface/CustomWidget.h>

class AboutWindow
	: public CustomWidget
{
	public:
		AboutWindow();
		virtual ~AboutWindow() {};

	private:
		void InitUI();
};