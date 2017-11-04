#include <pch.h>
#include "ObjectPropertiesWindow.h"

#include <Editor/UI/DockWindowManager.h>
#include <Editor/Interface/QtSimpleWidgets.h>

// QT
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QStyledItemDelegate>

using namespace std;

static GameObject* defaultObject;

ObjectPropertiesWindow::ObjectPropertiesWindow()
{
	Configure("ObjectProperties");
	forceUpdate = false;
	defaultObject = new GameObject();
	defaultObject->SetName("NO SELECTION");
	context = defaultObject;

	SubscribeToEvent(EventType::EDITOR_OBJECT_SELECTED);
	SubscribeToEvent(EventType::EDITOR_NO_SELECTION);
	SubscribeToEvent(EventType::FRAME_END);

	appChannel = new ObjectPropertiesUIChannel();
	GUI::GUIControl::RegisterChannel(appChannel, GUI::CommChannel::OBJECT_PROP);
}

void ObjectPropertiesWindow::InitUI()
{
	SetScrollable(true);

	qtLayout->setSpacing(5);
	qtLayout->setAlignment(Qt::AlignTop);

	{
		objectName = new SimpleTextBox("Name");
		objectName->OnUserEdit([this](string value) {
			context->SetName(std::move(value));
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


	worldPosition = new GLMVecComponent<glm::vec3>("Position", glm::vec3());
	worldPosition->OnUserEdit([this](glm::vec3 val) {
		context->transform->SetWorldPosition(val);
	});
	qtLayout->addWidget(worldPosition);

	worldScale = new GLMVecComponent<glm::vec3>("Scale", glm::vec3());
	worldScale->OnUserEdit([this](glm::vec3 val) {
		context->transform->SetScale(val);
	});
	qtLayout->addWidget(worldScale);

	worldEuler = new GLMVecComponent<glm::vec3>("Rotation", glm::vec3());
	worldEuler->OnUserEdit([this](glm::vec3 val) {
		context->transform->SetWorldRotation(val);
	});
	qtLayout->addWidget(worldEuler);

	worldQuat = new GLMVecComponent<glm::quat>("Quat", glm::quat());
	worldQuat->OnUserEdit([this](glm::quat val) {
		context->transform->SetWorldRotation(val);
	});
	qtLayout->addWidget(worldQuat);

	{
		auto zone = new QLabel("Relative properties");
		zone->setAlignment(Qt::AlignCenter);
		zone->setMargin(5);
		qtLayout->addWidget(zone);
	}

	localPosition = new GLMVecComponent<glm::vec3>("Local pos", glm::vec3());
	localPosition->OnUserEdit([this](glm::vec3 val) {
		context->transform->SetLocalPosition(val);
	});
	qtLayout->addWidget(localPosition);

	localEuler = new GLMVecComponent<glm::vec3>("Local rot", glm::vec3());
	localEuler->OnUserEdit([this](glm::vec3 val) {
		std::cout << val << std::endl;
		context->transform->SetRelativeRotation(val);
	});
	qtLayout->addWidget(localEuler);

	localQuat = new GLMVecComponent<glm::quat>("Local quat", glm::quat());
	localQuat->OnUserEdit([this](glm::quat val) {
		context->transform->SetRelativeRotation(val);
	});
	qtLayout->addWidget(localQuat);

	limitAxis = new GLMVecComponent<glm::vec3>("Limit axis", glm::vec3());
	limitAxis->OnUserEdit([this](glm::vec3 value) {
		auto T = dynamic_cast<LimitedTransform*>(context->transform);
		if (T) T->LimitAxisRotation(value);
	});
	qtLayout->addWidget(limitAxis);

	limitAxis = new GLMVecComponent<glm::vec3>("Limit axis", glm::vec3());
	limitAxis->OnUserEdit([this](glm::vec3 value) {
		auto T = dynamic_cast<LimitedTransform*>(context->transform);
		if (T) T->LimitAxisRotation(value);
	});
	qtLayout->addWidget(limitAxis);

	#ifdef PHYSICS_ENGINE
	{
		auto W = new CustomWidget(QBoxLayout::LeftToRight);
		body->AddWidget(W);

		{
			auto button = new QPushButton("Use Physics");
			QObject::connect(button, &QPushButton::clicked, this, [this]() {
				contextUpdate++;
				EmitEvent("Game", "UsePhysics", context);
			});
			W->AddWidget(button);
			addPhysicsBtn = button;
		}

		{
			auto button = new QPushButton("Remove Physics");
			QObject::connect(button, &QPushButton::clicked, this, [this]() {
				contextUpdate++;
				EmitEvent("Game", "RemovePhysics", context);
			});
			W->AddWidget(button);
			removePhysicsBtn = button;
		}

		auto W2 = new CustomWidget(QBoxLayout::LeftToRight);
		body->AddWidget(W2);

		{
			auto checkbox = new SimpleCheckBox("Dynamic");
			checkbox->OnUserEdit([this](bool value) {
				hasGravity->setEnabled(value);
				EmitEvent("Game", value ? "SetIsDynamicTrue" : "SetIsDynamicFalse", context);
			});
			isDyanmic = checkbox;
			W2->AddWidget(checkbox);
		}

		{
			auto checkbox = new SimpleCheckBox("Gravity");
			checkbox->OnUserEdit([this](bool value) {
				EmitEvent("Game", value ? "SetGravityTrue" : "SetGravityFalse", context);
			});
			hasGravity = checkbox;
			W2->AddWidget(checkbox);
		}

		auto W3 = new CustomWidget(QBoxLayout::LeftToRight);
		body->AddWidget(W3);

		{
			auto checkbox = new SimpleCheckBox("Kinematic");
			checkbox->OnUserEdit([this](bool value) {
				EmitEvent("Game", value ? "SetIsKinematicTrue" : "SetIsKinematicFalse", context);
			});
			isKinematic = checkbox;
			W3->AddWidget(checkbox);
		}

		{
			auto checkbox = new SimpleCheckBox("Trigger");
			checkbox->OnUserEdit([this](bool value) {
				EmitEvent("Game", value ? "SetIsTriggerTrue" : "SetIsTriggerFalse", context);
			});
			isTrigger = checkbox;
			W3->AddWidget(checkbox);
		}

		{
			auto floatInput = new SimpleFloatInput("Mass", "Kg", 3);
			floatInput->AcceptNegativeValues(false);
			floatInput->OnUserEdit([this](float value) {
				appChannel->context = context;
				appChannel->physicsMass = value;
				EmitEvent("Game", "SetPhysicsMass", appChannel);
			});
			objectMass = floatInput;
			body->AddWidget(floatInput);
		}
	}
	#endif // PHYSICS_ENGINE

}

void ObjectPropertiesWindow::UpdateUI()
{
	if (contextUpdate > 0) {
		contextUpdate--;
		auto T = dynamic_cast<LimitedTransform*>(context->transform);
		T ? limitAxis->Show() : limitAxis->Hide();

		#ifdef PHYSICS_ENGINE
		{
			auto activePhysics = false;
			auto hasPhysics = context->physics != nullptr;
			if (hasPhysics) {
				activePhysics = context->physics->IsActive();

				isDyanmic->SetValue(context->physics->IsDynamic(), false);
				isTrigger->SetValue(context->physics->IsTrigger(), false);
				isKinematic->SetValue(context->physics->IsKinematic(), false);
			}
			isDyanmic->setEnabled(activePhysics);
			hasGravity->setEnabled(activePhysics);
			isTrigger->setEnabled(activePhysics);
			isKinematic->setEnabled(activePhysics);
			removePhysicsBtn->setEnabled(activePhysics);
		}
		#endif
	}

	if (forceUpdate || shouldUpdate) {
		objectName->SetValue(context->GetName(), false);
		context->transform->GetWorldPosition();
		worldPosition->SetValue(context->transform->GetWorldPosition());
		localPosition->SetValue(context->transform->GetLocalPosition());

		worldScale->SetValue(context->transform->GetScale());

		worldQuat->SetValue(context->transform->GetWorldRotation());
		localQuat->SetValue(context->transform->GetRelativeRotation());

		worldEuler->SetValue(context->transform->GetRotationEuler360());
		localEuler->SetValue(TO_DEGREES * glm::eulerAngles(context->transform->GetRelativeRotation()));
	}

	if (forceUpdate) {
		forceUpdate = false;
		auto value = qVariantFromValue((void*)(context->GetMesh()));
		meshType->SetValue(value, false);
	}

	shouldUpdate = false;
}

void ObjectPropertiesWindow::UpdateView(GameObject *ctx)
{
	contextUpdate++;
	context = ctx ? ctx : defaultObject;
	forceUpdate = true;
}

void ObjectPropertiesWindow::OnEvent(EventType Event, void * data)
{
	if (EventType::EDITOR_OBJECT_SELECTED == Event) {
		auto ctx = dynamic_cast<GameObject*>((GameObject*)data);
		UpdateView(ctx);
	}
	if (EventType::EDITOR_NO_SELECTION == Event) {
		UpdateView(nullptr);
	}
	if (EventType::FRAME_END == Event) {
		shouldUpdate = context->transform->GetMotionState();
	}
}
