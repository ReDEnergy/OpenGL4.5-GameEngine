#pragma once
#include <string>

#include <Editor/include/QTForward.h>
#include <QWidget>
#include <QBoxLayout>

using namespace std;

class CustomWidget
	: public QWidget
{
	public:
		CustomWidget(QBoxLayout::Direction direction = QBoxLayout::Direction::TopToBottom);
		virtual ~CustomWidget() {};

		virtual void Init();
		virtual void Hide();
		virtual void Toggle();
		virtual void Close() {};

		void AddWidget(QWidget *widget);
		void DetachFromParent();
		void SetAsToolWindow();
		void SetIcon(const char* fileName);
		void SetStyleID(const char* ID);

		void ReloadStyleSheet();
		void LoadStyleSheet(const char* file);

	protected:
		string styleSheet;
		QBoxLayout* qtLayout;
};