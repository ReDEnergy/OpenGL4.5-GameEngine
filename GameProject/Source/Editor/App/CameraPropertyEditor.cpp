#include <pch.h>
#include "CameraPropertyEditor.h"

#include <functional>

#include <Editor/UI/DockWindowManager.h>
#include <Editor/Interface/QtSimpleWidgets.h>

// QT
#include <QLayout>
#include <QLabel>
#include <QKeyEvent>

CameraPropertyEditor::CameraPropertyEditor()
{
	Configure("CameraProperties");
	shouldUpdate = false;
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
		cameraName = new SimpleTextBox("Camera", "", true);
		qtLayout->addWidget(cameraName);
	}

	{
		cameraPosition = new GLMVecComponent<glm::vec3>("Position", glm::vec3());
		cameraPosition->OnUserEdit([](glm::vec3 val) {
			Manager::GetScene()->GetActiveCamera()->transform->SetWorldPosition(val);
		});
		qtLayout->addWidget(cameraPosition);

		cameraRotation = new GLMVecComponent<glm::vec3>("Rotation", glm::vec3());
		cameraRotation->OnUserEdit([](glm::vec3 val) {
			Manager::GetScene()->GetActiveCamera()->transform->SetWorldRotation(val);
		});
		qtLayout->addWidget(cameraRotation);

		camerFoV = new SimpleFloatInput("H-FoV", " degrees", 0);
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
			if (activeCamera) {
				auto PI = activeCamera->GetProjectionInfo();
				PI.FoVy = value;
				activeCamera->SetProjection(PI);
			}
		});

		qtLayout->addWidget(camerFoV);

		moveSpeed = new SimpleFloatInput("Move speed", " m/s", 2);
		moveSpeed->AcceptNegativeValues(false);
		moveSpeed->OnUserEdit([](float value) {
			Manager::GetScene()->GetActiveCamera()->transform->SetMoveSpeed(value);
		});
		qtLayout->addWidget(moveSpeed);
	}
}

void CameraPropertyEditor::InitUIState()
{
	auto cam = Manager::GetScene()->GetActiveCamera();
	moveSpeed->SetValue(cam->transform->GetMoveSpeed());
}

void CameraPropertyEditor::UpdateUI()
{
	if (!shouldUpdate) return;
	shouldUpdate = false;

	auto cameraTransform = activeCamera->transform;

	if (updateNewCamera)
	{
		updateNewCamera = false;
		cameraName->SetValue(activeCamera->GetName());
	}
	
	cameraPosition->SetValue(cameraTransform->GetWorldPosition());
	cameraRotation->SetValue(cameraTransform->GetRotationEuler360());
	camerFoV->SetValue(activeCamera->GetFieldOfViewY());
}

void CameraPropertyEditor::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::FRAME_END) {
		auto camera = Manager::GetScene()->GetActiveCamera();
		if (shouldUpdate) {
			return;
		}
		if (activeCamera != camera) {
			activeCamera = camera;
			shouldUpdate = true;
			updateNewCamera = true;
		} 
		else {
			shouldUpdate = camera->transform->GetMotionState();
		}
	}
}