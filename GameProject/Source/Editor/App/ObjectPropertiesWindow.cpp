#include <pch.h>
#include "ObjectPropertiesWindow.h"

#include <functional>

#include <Editor/UI/GUI.h>
#include <Editor/Interface/SimpleTextBox.h>

// QT
#include <QComboBox>
#include <QLayout>
#include <QPushButton>
#include <QStyledItemDelegate>

static GameObject* defaultObject;

ObjectPropertiesWindow::ObjectPropertiesWindow()
{
	setWindowTitle("Object");
	forceUpdate = false;
	defaultObject = new GameObject();
	context = defaultObject;

	InitUI();

	SubscribeToEvent(EventType::EDITOR_OBJECT_SELECTED);
	SubscribeToEvent(EventType::EDITOR_NO_SELECTION);
	SubscribeToEvent(EventType::FRAME_END);
}

void ObjectPropertiesWindow::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setAlignment(Qt::AlignTop);

	{
		objectName = new SimpleTextBox("Name");
		objectName->OnUserEdit([this](const char* value) {
			context->SetName(value);
		});
		qtLayout->addWidget(objectName);
	}

	{
		meshType = new SimpleDropDown("3D Mesh");

		auto meshList = Manager::GetResource()->GetMeshList();
		for (auto &mesh : meshList) {
			auto value = qVariantFromValue((void*)(mesh.second));
			meshType->AddOption(mesh.first.c_str(), value);
		}

		meshType->OnChange([this](QVariant data) {
			auto mesh = (Mesh*)(data.value<void*>());
			context->SetMesh(mesh);
		});

		qtLayout->addWidget(meshType);
	}

	{
		auto zone = new QLabel("Absolute properties");
		zone->setAlignment(Qt::AlignCenter);
		zone->setMargin(5);
		qtLayout->addWidget(zone);
	}

	{
		worldPosition = new GLMVecComponent<glm::vec3>("Position:", glm::vec3());
		worldPosition->OnUserEdit([this](glm::vec3 val) {
			context->transform->SetWorldPosition(val);
		});
		qtLayout->addWidget(worldPosition);

		worldEuler = new GLMVecComponent<glm::vec3>("Rotation:", glm::vec3());
		worldEuler->OnUserEdit([this](glm::vec3 val) {
			context->transform->SetWorldRotation(val);
		});
		qtLayout->addWidget(worldEuler);

		worldScale = new GLMVecComponent<glm::vec3>("Scale:", glm::vec3());
		worldScale->OnUserEdit([this](glm::vec3 val) {
			context->transform->SetScale(val);
		});
		qtLayout->addWidget(worldScale);

		worldQuat = new GLMVecComponent<glm::quat>("Quat:", glm::quat());
		worldQuat->OnUserEdit([this](glm::quat val) {
			context->transform->SetWorldRotation(val);
		});
		qtLayout->addWidget(worldQuat);
	}
}

void ObjectPropertiesWindow::Update()
{
	bool selectedMotion = context->transform->GetMotionState();
	auto cameraTransform = Manager::GetScene()->GetActiveCamera()->transform;
	bool cameraMotion = cameraTransform->GetMotionState();

	if (forceUpdate || selectedMotion) {
		objectName->SetValue(context->GetName());
		context->transform->GetWorldPosition();
		worldPosition->SetValue(context->transform->GetWorldPosition());
		worldQuat->SetValue(context->transform->GetWorldRotation());
		worldScale->SetValue(context->transform->GetScale());
		worldEuler->SetValue(context->transform->GetRotationEuler360());
	}

	if (forceUpdate) {
		auto value = qVariantFromValue((void*)(context->GetMesh()));
		meshType->SetValue(value);
	}
}

void ObjectPropertiesWindow::ForceUpdate()
{
	if (forceUpdate == false)
	{
		forceUpdate = true;
		Update();
		forceUpdate = false;
	}
}

void ObjectPropertiesWindow::OnEvent(EventType Event, void * data)
{
	if (EventType::EDITOR_OBJECT_SELECTED == Event) {
		context = dynamic_cast<GameObject*>((GameObject*)data);
		if (!context)
			context = defaultObject;
		ForceUpdate();
	}
	if (EventType::EDITOR_NO_SELECTION == Event) {
		context = defaultObject;
		ForceUpdate();
	}
	if (EventType::FRAME_END == Event) {
		Update();
	}
}
