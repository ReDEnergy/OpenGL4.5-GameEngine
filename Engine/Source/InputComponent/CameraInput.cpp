#include "CameraInput.h"
#include <iostream>

#include <include/gl.h>
#include <Component/Transform/Transform.h>

#include <Core/WindowObject.h>
#include <Core/Camera/Camera.h>

using namespace std;

CameraInput::CameraInput(Camera *camera)
{
	this->camera = camera;
}

void CameraInput::OnInputUpdate(float deltaTime, int mods)
{
	if (!window->MouseHold(MOUSE_BUTTON::RIGHT)) return;

	if (window->KeyHold(GLFW_KEY_W))
		camera->MoveForward(deltaTime);
	if (window->KeyHold(GLFW_KEY_S))
		camera->MoveBackward(deltaTime);
	if (window->KeyHold(GLFW_KEY_A))
		camera->MoveLeft(deltaTime);
	if (window->KeyHold(GLFW_KEY_D))
		camera->MoveRight(deltaTime);
	if (window->KeyHold(GLFW_KEY_Q))
		camera->MoveDown(deltaTime);
	if (window->KeyHold(GLFW_KEY_E))
		camera->MoveUp(deltaTime);

	if (window->KeyHold(GLFW_KEY_KP_MULTIPLY))
		camera->UpdateSpeed();
	if (window->KeyHold(GLFW_KEY_KP_DIVIDE))
		camera->UpdateSpeed(-0.2f);

	if (window->KeyHold(GLFW_KEY_KP_4))
		camera->RotateOY( 500 * deltaTime);
	if (window->KeyHold(GLFW_KEY_KP_6))
		camera->RotateOY(-500 * deltaTime);
	if (window->KeyHold(GLFW_KEY_KP_8))
		camera->RotateOX( 700 * deltaTime);
	if (window->KeyHold(GLFW_KEY_KP_5))
		camera->RotateOX(-700 * deltaTime);

	camera->Update();
}

void CameraInput::OnKeyPress(int key, int mods) {
	if (mods) return;
	if (key == GLFW_KEY_C)
		camera->Log();
}

void CameraInput::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {

	if (((window->GetSpecialKeyState() == SPECIAL_KEY::ALT) && window->MouseHold(MOUSE_BUTTON::LEFT)) ||
		window->MouseHold(MOUSE_BUTTON::RIGHT)) 
	{
		camera->RotateOY(-(float)deltaX);
		camera->RotateOX(-(float)deltaY);
		camera->Update();
	}
}

void CameraInput::OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods)
{
	if (action == GLFW_RELEASE) {
		if ((button == MOUSE_BUTTON::LEFT && window->MouseHold(MOUSE_BUTTON::RIGHT) == false) || button == MOUSE_BUTTON::RIGHT) {
			window->ShowPointer();
		}
		return;
	}

	if (action == GLFW_PRESS && ((mods == SPECIAL_KEY::ALT && button == MOUSE_BUTTON::LEFT) || (button == MOUSE_BUTTON::RIGHT)))
	{
		window->DisablePointer();
	}
}