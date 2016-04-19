#include "CustomWidget.h"
#include <iostream>

#include <QtWidgets/QBoxLayout>
#include <QtCore/QFile>
#include <QIcon>

#include <Editor/Config/QtConfig.h>

using namespace std;

static unsigned int windowNumber = 0;

CustomWidget::CustomWidget(QBoxLayout::Direction direction)
{
	SetIcon("colorwheel.png");

	qtLayout = new QBoxLayout(direction);
	qtLayout->setMargin(0);
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(0, 0, 0, 0);

	this->setLayout(qtLayout);

	styleSheet = "";
}

void CustomWidget::Init()
{
}

void CustomWidget::Hide()
{
	hide();
}

void CustomWidget::Toggle()
{
	if (isVisible())
		Hide();
	else
		show();
}

void CustomWidget::AddWidget(QWidget * widget)
{
	if (widget)
		qtLayout->addWidget(widget);
}

void CustomWidget::DetachFromParent()
{
	QWidget *widgetParent = (QWidget*)parent();
	if (widgetParent) {
		widgetParent->layout()->removeWidget(this);
		this->setParent(nullptr);
	}
}

void CustomWidget::SetAsToolWindow()
{
	setWindowFlags(Qt::WindowType::Tool | Qt::WindowType::WindowStaysOnTopHint);
}

void CustomWidget::SetIcon(const char * fileName)
{
	setWindowIcon(*QtConfig::GetIcon(fileName));
}

void CustomWidget::SetStyleID(const char * ID)
{
	setObjectName(ID);
}

void CustomWidget::ReloadStyleSheet()
{
	if (styleSheet.size())
		LoadStyleSheet(styleSheet.c_str());
}

void CustomWidget::LoadStyleSheet(const char* file)
{
	styleSheet = file;
	QtConfig::SetWidgetStyleSheet(this, file);
}