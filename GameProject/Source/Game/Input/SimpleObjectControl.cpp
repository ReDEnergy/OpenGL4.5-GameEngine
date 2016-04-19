#include <pch.h>

#include <include/gl.h>
#include "SimpleObjectControl.h"

SimpleObjectControl::SimpleObjectControl(GameObject *obj)
	: GO(obj)
{
	GO->transform->SetMoveSpeed(5);
}

SimpleObjectControl::~SimpleObjectControl()
{

}

void SimpleObjectControl::Update(float deltaTime, int mods)
{
	if (mods != GLFW_MOD_SHIFT) return;

	if (window->KeyHold(GLFW_KEY_D)) {
		GO->transform->RotateWorldOY(-deltaTime);
	}
	if (window->KeyHold(GLFW_KEY_A)) {
		GO->transform->RotateWorldOY(deltaTime);
	}
	if (window->KeyHold(GLFW_KEY_W)) {
		GO->transform->Move(GO->transform->GetLocalOXVector(), deltaTime);
	}
	if (window->KeyHold(GLFW_KEY_S)) {
		GO->transform->Move(GO->transform->GetLocalOXVector(), -deltaTime);
	}
}

void SimpleObjectControl::OnKeyPress(int key, int mods)
{
}