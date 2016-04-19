#include <pch.h>
#include "CameraPropertyEditor.h"

#include <functional>

#include <Editor/UI/GUI.h>

// QT
#include <QLayout>
#include <QKeyEvent>

CameraPropertyEditor::CameraPropertyEditor()
{
	setWindowTitle("Camera Properties");
	LoadStyleSheet("camera-properties.css");

	InitUI();
	SubscribeToEvent(EventType::FRAME_END);
}

void CameraPropertyEditor::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(0, 0, 0, 0);

	{
		auto zone = new QLabel("Camera properties");
		zone->setAlignment(Qt::AlignCenter);
		zone->setProperty("title", 0);
		qtLayout->addWidget(zone);
	}

	{
		cameraPosition = new GLMVecComponent<glm::vec3>("Position:", glm::vec3());
		cameraPosition->OnUserEdit([](glm::vec3 val) {
			Manager::GetScene()->GetActiveCamera()->transform->SetWorldPosition(val);
		});
		qtLayout->addWidget(cameraPosition);

		cameraRotation = new GLMVecComponent<glm::vec3>("Rotation:", glm::vec3());
		cameraRotation->OnUserEdit([](glm::vec3 val) {
			Manager::GetScene()->GetActiveCamera()->transform->SetWorldRotation(val);
		});
		qtLayout->addWidget(cameraRotation);

		camerFoV = new SimpleFloatInput("Camera FoV", "degrees", 0);
		camerFoV->AcceptNegativeValues(false);
		camerFoV->OnUserEdit([this](float value) {
			if (value < 10) {
				camerFoV->SetValue(10);
				return;
			}
			if (value > 120) {
				camerFoV->SetValue(120);
				return;
			}
			auto cam = Manager::GetScene()->GetActiveCamera();
			auto PI = cam->GetProjectionInfo();
			PI.FoV = max(10.0f, value) / PI.aspectRatio;
			cam->SetProjection(PI);
		});

		auto PI = Manager::GetScene()->GetActiveCamera()->GetProjectionInfo();
		camerFoV->SetValue(PI.FoV * PI.aspectRatio);
		qtLayout->addWidget(camerFoV);

		moveSpeed = new SimpleFloatInput("Move spped", "meters/second", 2);
		moveSpeed->AcceptNegativeValues(false);
		moveSpeed->OnUserEdit([](float value) {
			Manager::GetScene()->GetActiveCamera()->transform->SetMoveSpeed(value);
		});
		auto cam = Manager::GetScene()->GetActiveCamera();
		moveSpeed->SetValue(cam->transform->GetMoveSpeed());
		qtLayout->addWidget(moveSpeed);
	}
}

void CameraPropertyEditor::Update()
{
	auto cameraTransform = Manager::GetScene()->GetActiveCamera()->transform;
	bool cameraMotion = cameraTransform->GetMotionState();

	if (cameraMotion) {
		cameraPosition->SetValue(cameraTransform->GetWorldPosition());
		cameraRotation->SetValue(cameraTransform->GetRotationEuler360());
	}
}

void CameraPropertyEditor::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::FRAME_END) {
		Update();
	}
}