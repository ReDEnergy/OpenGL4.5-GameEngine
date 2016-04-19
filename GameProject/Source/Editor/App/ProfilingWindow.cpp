#include <pch.h>
#include "ProfilingWindow.h"

#include <functional>

#include <Editor/UI/GUI.h>
#include <Editor/Interface/SimpleTextBox.h>

// QT
#include <QComboBox>
#include <QLayout>
#include <QPushButton>
#include <QStyledItemDelegate>

static GameObject* defaultObject;

ProfilingWindow::ProfilingWindow()
{
	setWindowTitle("Profiler");

	profileInfo = nullptr;

	InitUI();

	LoadStyleSheet("profiling-window.css");
	SubscribeToEvent("Set-Profile-Timer");
	SubscribeToEvent(EventType::FRAME_END);
}

void ProfilingWindow::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setAlignment(Qt::AlignTop);

	{
		auto zone = new QLabel("Profiler");
		zone->setObjectName("Title");
		zone->setAlignment(Qt::AlignCenter);
		zone->setMargin(5);
		qtLayout->addWidget(zone);
	}
}

void ProfilingWindow::UpdateUI()
{
	if (!profileInfo) return;

	// Clear UI
	for (auto channel : channels) {
		channel->DetachFromParent();
		SAFE_FREE(channel);
	}
	channels.clear();

	auto times = profileInfo->GetInfo();
	auto size = times.size();
	for (unsigned int i = 1; i < size; i++) {
		auto input = new SimpleFloatInput(times[i].label, "ms", 2);
		channels.push_back(input);
		qtLayout->addWidget(input);
	}

	{
		auto input = new SimpleFloatInput("Frame Time", "ms", 2);
		input->setObjectName("TotalTime");
		channels.push_back(input);
		qtLayout->addWidget(input);
	}

}

void ProfilingWindow::Update()
{
	if (!profileInfo) return;
	auto times = profileInfo->GetInfo();
	auto size = times.size();

	if (channels.size() != size) {
		UpdateUI();
	}

	for (unsigned int i = 1; i < size; i++) {
		channels[i - 1]->SetValue(1000 * (times[i].time - times[i - 1].time));
	}
	channels[size - 1]->SetValue(1000 * (times[size - 1].time - times[0].time));
}

void ProfilingWindow::OnEvent(EventType Event, void * data)
{
	if (EventType::FRAME_END == Event) {
		if (profileInfo->IsActive())
			Update();
	}
}

void ProfilingWindow::OnEvent(const string & eventID, void * data)
{
	if (eventID.compare("Set-Profile-Timer") == 0) {
		profileInfo = (ProfileTimer*)data;
		UpdateUI();
	}
}
