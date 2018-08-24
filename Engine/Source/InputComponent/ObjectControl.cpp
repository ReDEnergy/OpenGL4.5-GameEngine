#include <iostream>

#include "ObjectControl.h"

#include <include/gl.h>
#include <include/glm.h>
#include <include/glm_utils.h>

#include <Core/InputSystem.h>
#include <Core/WindowObject.h>
#include <Component/Transform/Transform.h>

using namespace std;

ObjectControl::ObjectControl(Transform *transform)
	: transform(transform)
{
	controlMode = ControlMode::TRANSLATE;
}

void ObjectControl::OnKeyPress(int key, int mods)
{
	// cout << "key: " << key << " mods: " << mods << endl;

	if (mods != GLFW_MOD_SHIFT)
		return;

	switch (key)
	{
		case GLFW_KEY_R:
			controlMode = ControlMode::Scale;
			cout << "ControlMode: Scale" << endl;
			return;
		case GLFW_KEY_E:
			controlMode = ControlMode::Rotate;
			cout << "ControlMode: Rotate" << endl;
			return;
		case GLFW_KEY_W:
			controlMode = ControlMode::TRANSLATE;
			cout << "ControlMode: TRANSLATE" << endl;
			return;
		case GLFW_KEY_KP_3:
			cout << "position" << transform->GetWorldPosition() << endl;
			cout << "rotation" << transform->GetRotationEulerRad() << endl;
			cout << "scale" << transform->GetScale() << endl;
			return;
	}
}

void ObjectControl::OnInputUpdate(float deltaTime, int mods)
{
	// Translate
	if (mods != GLFW_MOD_SHIFT)
		return;
	if (controlMode == ControlMode::TRANSLATE) {
		if (window->KeyHold(GLFW_KEY_KP_5)) {
			transform->Move(glm::vec3_forward, deltaTime);
		}
		if (window->KeyHold(GLFW_KEY_KP_8)) {
			transform->Move(glm::vec3_backward, deltaTime);
		}
		if (window->KeyHold(GLFW_KEY_KP_9)) {
			transform->Move(glm::vec3_up, deltaTime);
		}
		if (window->KeyHold(GLFW_KEY_KP_7)) {
			transform->Move(glm::vec3_down, deltaTime);
		}
		if (window->KeyHold(GLFW_KEY_KP_4)) {
			transform->Move(glm::vec3_left, deltaTime);
		}
		if (window->KeyHold(GLFW_KEY_KP_6)) {
			transform->Move(glm::vec3_right, deltaTime);
		}
	}

	// Rotate
	if (controlMode == ControlMode::Rotate) {
		if (window->KeyHold(GLFW_KEY_KP_7)) {
			transform->RotateLocalOZ(deltaTime);
		}
		if (window->KeyHold(GLFW_KEY_KP_9)) {
			transform->RotateLocalOZ(-deltaTime);
		}
		if (window->KeyHold(GLFW_KEY_KP_4)) {
			transform->RotateLocalOY(-deltaTime);
		}
		if (window->KeyHold(GLFW_KEY_KP_6)) {
			transform->RotateLocalOY(deltaTime);
		}
		if (window->KeyHold(GLFW_KEY_KP_8)) {
			transform->RotateLocalOX(-deltaTime);
		}
		if (window->KeyHold(GLFW_KEY_KP_5)) {
			transform->RotateLocalOX(deltaTime);
		}
	}

	// Scale
	if (controlMode == ControlMode::Scale) {
		if (window->KeyHold(GLFW_KEY_KP_ADD)) {
			transform->Scale(deltaTime);
		}
		if (window->KeyHold(GLFW_KEY_KP_SUBTRACT)) {
			transform->Scale(-deltaTime);
		}
	}
}