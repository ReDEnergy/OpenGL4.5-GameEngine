#include "DockWindow.h"
#include <iostream>

#include <QtWidgets/QBoxLayout>
#include <QtCore/QFile>
#include <Editor/Config/QtConfig.h>

using namespace std;

static unsigned int windowNumber = 0;

DockWindow::DockWindow()
{
	setWindowTitle("DockWindow");
	setMinimumWidth(120);
	setMinimumHeight(150);

	qtLayout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(0, 0, 0, 0);
	qtLayout->setAlignment(Qt::AlignTop);

	QWidget* layoutWidget = new QWidget();
	layoutWidget->setLayout(qtLayout);
	this->setWidget(layoutWidget);

	QObject::connect(this, &QDockWidget::topLevelChanged, this, &DockWindow::DockedEvent);

	LoadStyleSheet("app.css");
}

void DockWindow::Init()
{
}

void DockWindow::Hide()
{
	hide();
}

void DockWindow::ReloadStyleSheet()
{
	if (styleSheet.size())
		LoadStyleSheet(styleSheet.c_str());
}

void DockWindow::LoadStyleSheet(const char * file)
{
	styleSheet = file;
	QtConfig::SetWidgetStyleSheet(this, file);
}

void DockWindow::DockedEvent(bool state)
{
	cout << state << endl;
}
