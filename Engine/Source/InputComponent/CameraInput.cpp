//#include <pch.h>
#include "CameraInput.h"

#include <Core/InputSystem.h>
#include <Core/Camera/Camera.h>

CameraInput::CameraInput(Camera *camera)
	: ObjectInput(InputGroup::IG_ENGINE)
{
	this->camera = camera;
}

void CameraInput::Update(float deltaTime, int mods) {
	if (mods) return;

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
		camera->IncreaseSpeed();
	if (InputSystem::KeyHold(GLFW_KEY_KP_DIVIDE))
		camera->DecreaseSpeed();

	if (InputSystem::KeyHold(GLFW_KEY_KP_4))
		camera->RotateOY(-500 * deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_KP_6))
		camera->RotateOY( 500 * deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_KP_8))
		camera->RotateOX(-700 * deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_KP_5))
		camera->RotateOX( 700 * deltaTime);

	camera->Update();
}

void CameraInput::OnKeyPress(int key, int mods) {
	if (mods) return;
	if (key == GLFW_KEY_C)
		camera->Log();
}

void CameraInput::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
	//printf("camera mouse: %d %d %d %d\n", mouseX, mouseY, deltaX, deltaY);
	camera->RotateOY((float)deltaX);
	camera->RotateOX((float)deltaY);
	camera->Update();
}