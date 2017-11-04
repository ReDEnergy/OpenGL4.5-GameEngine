#pragma once
#include <string>

#include <Editor/include/QtForward.h>
#include <QtWidgets/QDockWidget>

class CustomWidget;

class DockWindow
	: public QDockWidget
{
	public:
		DockWindow();
		virtual ~DockWindow() {};

		virtual void Init() {};
		virtual void InitUI() {};
		virtual void Hide();
		virtual void Close() {};
		virtual void Update() final;
		virtual bool IsVisible();

		virtual void SetScrollable(bool state, bool resizableContent = true) final;
		virtual bool IsScrollable() final;

		void ReloadStyleSheet();
		void LoadStyleSheet(const char *file);

	protected:
		virtual void InitUIState() {};
		virtual void UpdateUI() {};

		virtual void DockedEvent(bool state);

	protected:
		bool isVisible;
		std::string styleSheet;
		QBoxLayout* qtLayout;
		CustomWidget *body;

	private:
		QScrollArea *scrollArea;
};