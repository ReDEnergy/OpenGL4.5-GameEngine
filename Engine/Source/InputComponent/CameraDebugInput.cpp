//#include <pch.h>
#include "CameraDebugInput.h"

#include <Core/InputSystem.h>
#include <Core/Camera/Camera.h>

CameraDebugInput::CameraDebugInput(Camera *camera)
	: ObjectInput(InputGroup::IG_ENGINE)
{
	this->camera = camera;
}

void CameraDebugInput::Update(float deltaTime, int mods) {
	if (mods != GLFW_MOD_ALT) return;

	if (InputSystem::KeyHold(GLFW_KEY_W))
		camera->MoveForward(deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_S))
		camera->MoveBackward(deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_A))
		camera->MoveLeft(deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_D))
		camera->MoveRight(deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_Q))
		camera->MoveDown(deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_E))
		camera->MoveUp(deltaTime);

	if (InputSystem::KeyHold(GLFW_KEY_KP_MULTIPLY))
		camera->UpdateSpeed();
	if (InputSystem::KeyHold(GLFW_KEY_KP_DIVIDE))
		camera->UpdateSpeed(-0.2f);

	if (InputSystem::KeyHold(GLFW_KEY_KP_4))
		camera->RotateOY( 500 * deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_KP_6))
		camera->RotateOY(-500 * deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_KP_8))
		camera->RotateOX(-700 * deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_KP_5))
		camera->RotateOX( 700 * deltaTime);

	camera->Update();
}