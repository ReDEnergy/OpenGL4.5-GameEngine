#include "CameraInput.h"
#include <iostream>

#include <include/gl.h>
#include <Component/Transform/Transform.h>

#include <Core/WindowObject.h>
#include <Core/Camera/Camera.h>

#include <Manager/Manager.h>
#include <UI/ColorPicking/ColorPicking.h>

using namespace std;

CameraInput::CameraInput(Camera *camera)
{
	this->camera = camera;
	rotateTPS = false;
}

void CameraInput::OnInputUpdate(float deltaTime, int mods)
{
	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) return;

	if (window->KeyHold(GLFW_KEY_W))	camera->MoveForward(deltaTime);
	if (window->KeyHold(GLFW_KEY_S))	camera->MoveBackward(deltaTime);
	if (window->KeyHold(GLFW_KEY_A))	camera->MoveLeft(deltaTime);
	if (window->KeyHold(GLFW_KEY_D))	camera->MoveRight(deltaTime);
	if (window->KeyHold(GLFW_KEY_Q))	camera->MoveDown(deltaTime);
	if (window->KeyHold(GLFW_KEY_E))	camera->MoveUp(deltaTime);

	if (window->KeyHold(GLFW_KEY_KP_MULTIPLY))	camera->UpdateSpeed();
	if (window->KeyHold(GLFW_KEY_KP_DIVIDE))	camera->UpdateSpeed(-0.2f);

	if (window->KeyHold(GLFW_KEY_KP_4))		camera->RotateOY( 500 * deltaTime);
	if (window->KeyHold(GLFW_KEY_KP_6))		camera->RotateOY(-500 * deltaTime);
	if (window->KeyHold(GLFW_KEY_KP_8))		camera->RotateOX( 700 * deltaTime);
	if (window->KeyHold(GLFW_KEY_KP_5))		camera->RotateOX(-700 * deltaTime);

	camera->Update();
}

void CameraInput::OnKeyPress(int key, int mods) {
	if (mods) return;
	if (key == GLFW_KEY_C)
		camera->Log();
}

void CameraInput::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		auto selectedObject = Manager::GetPicker()->GetSelectedObject();
		if (rotateTPS && selectedObject) {
			if (selectedObject) {
				auto dist = camera->DistTo(*selectedObject);
				camera->RotateOY(-(float)deltaX);
				camera->RotateOX(-(float)deltaY);
				auto dir = camera->transform->GetLocalOZVector();
				camera->transform->SetWorldPosition(selectedObject->transform->GetWorldPosition() + dir * dist);
			}
		}
		else {
			camera->RotateOY(-(float)deltaX);
			camera->RotateOX(-(float)deltaY);
		}
		camera->Update();
	}
}

void CameraInput::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
	{
		rotateTPS = ((mods & GLFW_MOD_ALT) != 0);
		window->DisablePointer();
	}
}

void CameraInput::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
	{
		rotateTPS = ((mods & GLFW_MOD_ALT) != 0);
		window->ShowPointer();
	}
}