#include "DockWindow.h"
#include <iostream>

#include <Editor/Config/QtConfig.h>
#include <Editor/Windows/Interface/CustomWidget.h>

#include <QScrollArea>

using namespace std;

static unsigned int windowNumber = 0;

DockWindow::DockWindow()
{
	setWindowTitle("DockWindow");
	setObjectName("DockWindow");
	setMinimumWidth(120);
	setMinimumHeight(150);

	scrollArea = new QScrollArea();
	scrollArea->setObjectName("ScrollArea");

	body = new CustomWidget();
	qtLayout = (QBoxLayout*)body->layout();
	qtLayout->setAlignment(Qt::AlignTop);
	body->setLayout(qtLayout);
	body->setObjectName("Body");
	setWidget(body);

	QObject::connect(this, &QDockWidget::topLevelChanged, this, &DockWindow::DockedEvent);
	QObject::connect(this, &QDockWidget::visibilityChanged, this, [this](bool visible) {
		isVisible = visible;
	});

	LoadStyleSheet("app.css");
}

void DockWindow::Hide()
{
	hide();
}

void DockWindow::Update()
{
	if (isVisible)
		UpdateUI();
};

bool DockWindow::IsVisible()
{
	return isVisible;
}

void DockWindow::SetScrollable(bool state, bool resizableContent)
{
	bool isScrollable = IsScrollable();
	if (state == isScrollable)
		return;

	if (state) {
		setWidget(scrollArea);
		scrollArea->setWidget(body);
		scrollArea->setWidgetResizable(resizableContent);
	}
	else {
		scrollArea->takeWidget();
		scrollArea->setParent(nullptr);
		setWidget(body);
	}
	ReloadStyleSheet();
}

bool DockWindow::IsScrollable()
{
	return (scrollArea->parent() != nullptr);
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
	cout << "TOP LEVE CHANGE : " << state << endl;
}
