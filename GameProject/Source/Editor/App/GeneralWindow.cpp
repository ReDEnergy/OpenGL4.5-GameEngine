#include <pch.h>
#include "GeneralWindow.h"

#include <functional>

#include <Editor/UI/GUI.h>
#include <Editor/Windows/Interface/CustomWidget.h>

// QT
#include <QLabel>
#include <QLayout>
#include <QKeyEvent>
#include <QPushButton>

GeneralWindow::GeneralWindow()
{
	setWindowTitle("General Properties");
	LoadStyleSheet("general-properties.css");

	InitUI();
}

void GeneralWindow::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(0, 0, 0, 0);

	{
		auto zone = new QLabel("General properties");
		zone->setAlignment(Qt::AlignCenter);
		zone->setProperty("title", 0);
		qtLayout->addWidget(zone);
	}

	auto channelSelector = new CustomWidget(QBoxLayout::LeftToRight);
	string channels[4] = { "R", "G", "B", "A" };

	int index = 0;
	for (auto chn : channels) {
		auto button = new QPushButton(chn.c_str());
		button->setProperty("channel", chn.c_str());
		channelSelector->AddWidget(button);

		auto toggleFunc = bind(&GeneralWindow::ToggleMaskChannel, this, index++);
		QObject::connect(button, &QPushButton::clicked, this, toggleFunc);
	}

	qtLayout->addWidget(channelSelector);
}

void GeneralWindow::ToggleMaskChannel(int index)
{
	auto TBD = Manager::GetTextureDebugger();
	auto mask = TBD->GetMask();
	mask[index] = 1 - mask[index];
	TBD->SetViewMode(mask);
}
