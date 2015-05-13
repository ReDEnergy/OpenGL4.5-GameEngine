//#include <pch.h>
#include "ObjectControl.h"

#include <include/glm.h>
#include <include/glm_utils.h>

#include <Core/InputSystem.h>
#include <Component/Transform.h>

ObjectControl::ObjectControl(Transform *transform)
	: transform(transform),
	ObjectInput(InputGroup::IG_EDITOR)
{
	controlMode = ControlMode::TRANSLATE;
}

void ObjectControl::OnKeyPress(int key, int mods) {
	// cout << "key: " << key << " mods: " << mods << endl;

	if (mods != GLFW_MOD_SHIFT)
		return;

	switch (key)
	{
		case GLFW_KEY_R:
			controlMode = ControlMode::SCALE;
			cout << "ControlMode: SCALE" << endl;
			return;
		case GLFW_KEY_E:
			controlMode = ControlMode::ROTATE;
			cout << "ControlMode: ROTATE" << endl;
			return;
		case GLFW_KEY_W:
			controlMode = ControlMode::TRANSLATE;
			cout << "ControlMode: TRANSLATE" << endl;
			return;
		case GLFW_KEY_KP_3:
			cout << "position" << transform->position << endl;
			cout << "rotation" << transform->eulerAngles << endl;
			cout << "scale" << transform->scale << endl;
			return;
	}
}

void ObjectControl::Update(float deltaTime, int mods) {
	// Translate
	if (mods != GLFW_MOD_SHIFT)
		return;
	if (controlMode == ControlMode::TRANSLATE) {
		if (InputSystem::KeyHold(GLFW_KEY_KP_5)) {
			transform->Move(glm::vec3_forward, deltaTime);
		}
		if (InputSystem::KeyHold(GLFW_KEY_KP_8)) {
			transform->Move(glm::vec3_backward, deltaTime);
		}
		if (InputSystem::KeyHold(GLFW_KEY_KP_9)) {
			transform->Move(glm::vec3_up, deltaTime);
		}
		if (InputSystem::KeyHold(GLFW_KEY_KP_7)) {
			transform->Move(glm::vec3_down, deltaTime);
		}
		if (InputSystem::KeyHold(GLFW_KEY_KP_4)) {
			transform->Move(glm::vec3_left, deltaTime);
		}
		if (InputSystem::KeyHold(GLFW_KEY_KP_6)) {
			transform->Move(glm::vec3_right, deltaTime);
		}
	}

	// Rotate
	if (controlMode == ControlMode::ROTATE) {
		if (InputSystem::KeyHold(GLFW_KEY_KP_7)) {
			transform->RotatePitch(deltaTime);
		}
		if (InputSystem::KeyHold(GLFW_KEY_KP_9)) {
			transform->RotatePitch(-deltaTime);
		}
		if (InputSystem::KeyHold(GLFW_KEY_KP_4)) {
			transform->RotateYaw(-deltaTime);
		}
		if (InputSystem::KeyHold(GLFW_KEY_KP_6)) {
			transform->RotateYaw(deltaTime);
		}
		if (InputSystem::KeyHold(GLFW_KEY_KP_8)) {
			transform->RotateRoll(-deltaTime);
		}
		if (InputSystem::KeyHold(GLFW_KEY_KP_5)) {
			transform->RotateRoll(deltaTime);
		}
	}

	// Scale
	if (controlMode == ControlMode::SCALE) {
		if (InputSystem::KeyHold(GLFW_KEY_KP_ADD)) {
			transform->Scale(deltaTime);
		}
		if (InputSystem::KeyHold(GLFW_KEY_KP_SUBTRACT)) {
			transform->Scale(-deltaTime);
		}
	}
}