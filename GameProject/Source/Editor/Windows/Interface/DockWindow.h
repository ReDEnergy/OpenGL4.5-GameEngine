#pragma once
#include <string>

#include <Editor/include/QTForward.h>
#include <QtWidgets/QDockWidget>

using namespace std;

class DockWindow
	: public QDockWidget
{
	public:
		DockWindow();
		virtual ~DockWindow() {};

		virtual void Init();
		virtual void Hide();
		virtual void Close() {};
		virtual void Update() {};

		void ReloadStyleSheet();
		void LoadStyleSheet(const char *file);

	protected:
		virtual void DockedEvent(bool state);

	protected:
		string styleSheet;
		QBoxLayout* qtLayout;
};