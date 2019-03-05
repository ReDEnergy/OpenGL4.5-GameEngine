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
	sensitivityX = 700;
	sensitivityY = 500;
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
	if ((mods & GLFW_MOD_CONTROL) != 0) 		camera->UpdateSpeed(-deltaTime);
	if ((mods & GLFW_MOD_SHIFT) != 0)			camera->UpdateSpeed(deltaTime);

	if (window->KeyHold(GLFW_KEY_KP_4))		camera->RotateOY( sensitivityY * deltaTime);
	if (window->KeyHold(GLFW_KEY_KP_6))		camera->RotateOY(-sensitivityY * deltaTime);
	if (window->KeyHold(GLFW_KEY_KP_8))		camera->RotateOX( sensitivityX * deltaTime);
	if (window->KeyHold(GLFW_KEY_KP_5))		camera->RotateOX(-sensitivityX * deltaTime);

	camera->Update();
}

void CameraInput::OnKeyPress(int key, int mods)
{
	if (mods) return;
	if (key == GLFW_KEY_C)
		camera->Log();

	// ALT pressed
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		if ((mods & GLFW_MOD_ALT) != 0)
		{
			camera->transform->SetMoveSpeed(1);
		}
	}
}

void CameraInput::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)\
{
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) && window->KeyHold(GLFW_KEY_LEFT_ALT) == false)
	{
		camera->RotateOY(-(float)deltaX);
		camera->RotateOX(-(float)deltaY);
		camera->Update();
	}

	if (window->MouseHold(GLFW_MOUSE_BUTTON_MIDDLE))
	{
		float speed = 0.0025f;

		auto PI = camera->GetProjectionInfo();
		if (!PI.isPerspective) {
			speed *= PI.width / 5;
		}

		camera->MoveRight(-deltaX * speed);
		camera->MoveInDirection(camera->transform->GetLocalOYVector(), deltaY * speed);
		camera->Update();
	}
}

void CameraInput::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
	{
		window->DisablePointer();
	}
}

void CameraInput::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
	{
		window->ShowPointer();
	}
}

void CameraInput::OnMouseScroll(int mouseX, int mouseY, double offsetX, double offsetY)
{
	float zoomSpeed = 0.05f * static_cast<float>(offsetY);
	auto PI = camera->GetProjectionInfo();
	if (PI.isPerspective)
	{
		camera->MoveInDirection(-camera->transform->GetLocalOZVector(), zoomSpeed);
		camera->Update();
	}
	else
	{
		PI.width -= zoomSpeed * PI.aspectRatio;
		PI.height -= zoomSpeed;
		camera->SetProjection(PI);
	}
}