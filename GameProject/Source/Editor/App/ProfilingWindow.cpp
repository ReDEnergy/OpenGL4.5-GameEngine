#include <pch.h>
#include "ProfilingWindow.h"

#include <functional>

#include <Editor/UI/DockWindowManager.h>
#include <Editor/Interface/QtSimpleWidgets.h>
#include <Editor/Windows/Interface/CustomWidget.h>
#include <Runtime/Threading/utils.h>

// QT
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QScrollArea>

using namespace std;

static GameObject* defaultObject;

ProfilingWindow::ProfilingWindow()
{
	Configure("ProfilingWindow");

	newPerfInfo = false;
	profileInfo = new ProfilingTimer();

	SubscribeToEvent("Set-Profile-Timer");
	SubscribeToEvent(EventType::FRAME_END);
}

void ProfilingWindow::InitUI()
{
	SetScrollable(true);

	//auto scrollArea = new QScrollArea();
	//scrollArea->setObjectName("ScrollArea");
	//qtLayout->addWidget(scrollArea);

	qtLayout->setSpacing(5);
	qtLayout->setAlignment(Qt::AlignTop);

	//auto panel = new CustomWidget();
	//panel->layout()->setSizeConstraint(QLayout::SizeConstraint::SetMaximumSize);
	//panel->setObjectName("Content");
	//panel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	//scrollArea->setWidget(panel);
	//scrollArea->setWidgetResizable(true);
	//panel->resize(sizeHint());

	{
		auto zone = new QLabel("Profiler");
		zone->setObjectName("Title");
		zone->setAlignment(Qt::AlignCenter);
		zone->setMargin(5);
		body->AddWidget(zone);
	}

	profilingArea = new CustomWidget();
	body->AddWidget(profilingArea);

	frameInfoArea = new CustomWidget();
	frameInfoArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	body->AddWidget(frameInfoArea);

	{
		frameInfo = new SimpleTextBox("Frametime", "", true);
		frameInfo->setObjectName("FrameTime");
		frameInfoArea->AddWidget(frameInfo);
	}
}

void ProfilingWindow::UpdateList()
{
	auto times = profileInfo->GetInfo();
	uint newSize = static_cast<uint>(times.size() - 2);
	uint oldSize = static_cast<uint>(channels.size());

	// compute how many we have versus how many we need
	int diff = static_cast<int>((idleNodes.size() + channels.size()) - newSize);

	// add new nodes if needed
	while (diff < 0) {
		auto input = new SimpleFloatInput("unset", " ms", 2);
		idleNodes.push_back(input);
		diff++;
	}

	// Make sure that the UI is populated with the correct number of entries

	// More than we need, store them into the idleNodes
	if (oldSize > newSize) {
		for (uint i = newSize; i < oldSize; i++) {
			idleNodes.push_back(channels[i]);
			profilingArea->RemoveWidget(channels[i]);
		}
		channels.resize(newSize);
	}

	// less than we need, get them from the idleNodes
	if (oldSize < newSize) {
		for (uint i = oldSize; i < newSize; i++) {
			auto node = idleNodes.front();
			idleNodes.pop_front();
			channels.push_back(node);
			profilingArea->AddWidget(node);
		}
	}

	// Init the current nodes
	for (uint i = 0; i < newSize; i++) {
		channels[i]->SetLabel(std::to_string(times[i + 1].label).c_str());
		channels[i]->SetValue(times[i + 1].GetDeltaTime<std::milli>(times[i]));
	}
}

void ProfilingWindow::UpdateUI()
{
	if (!newPerfInfo) return;

	auto times = profileInfo->GetInfo();
	auto size = times.size() - 2;
	if (size == 0) return;

	if (channels.size() != size) {
		UpdateList();
	}
	else {
		for (uint i = 0; i < size; i++) {
			channels[i]->SetValue(times[i + 1].GetDeltaTime<std::milli>(times[i]));
		}
	}

	auto frameDelta = profileInfo->GetDeltaTime();
	sprintf(textBoxBuffer, "%.2f ms | %.2f fps", 1000 * frameDelta, 1 / frameDelta);
	frameInfo->SetValue(textBoxBuffer);
	newPerfInfo = false;
}

void ProfilingWindow::OnEvent(EventType Event, void * data)
{
	if (EventType::FRAME_END == Event) {
		if (newPerfInfo == false && appPerfInfo->IsActive()) {
			*profileInfo = *appPerfInfo;
			newPerfInfo = true;
		}
	}
}

void ProfilingWindow::OnEvent(const string & eventID, void * data)
{
	if (eventID.compare("Set-Profile-Timer") == 0) {
		appPerfInfo = (ProfilingTimer*)data;
	}
}
