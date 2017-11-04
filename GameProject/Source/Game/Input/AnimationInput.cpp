#include <pch.h>
#include "AnimationInput.h"

#include <include/gl.h>

using namespace std;

AnimationInput::AnimationInput(GameObject *obj)
	: GO(obj)
{
	GO->transform->SetMoveSpeed(2);
	GO->transform->SetRotationSpeed(200);
	//GO->animation->ScaleAnimationTime("Run", 1.5f);
	//GO->animation->ScaleAnimationTime("Idle", 0.3f);
	//GO->animation->ScaleAnimationTime("Attack", 0.5f);
	//GO->animation->ScaleAnimationTime("Die", 1.5f);
}

AnimationInput::~AnimationInput()
{
	
}

void AnimationInput::OnInputUpdate(float deltaTime, int mods)
{
	if (mods != GLFW_MOD_SHIFT)
		return;

	if (window->KeyHold(GLFW_KEY_D)) {
		GO->transform->RotateWorldOY(-deltaTime);
	}
	if (window->KeyHold(GLFW_KEY_A)) {
		GO->transform->RotateWorldOY(deltaTime);
	}
	if (window->KeyHold(GLFW_KEY_W)) {
		GO->transform->Move(glm::normalize(glm::rotate(GO->transform->GetWorldRotation(), glm::vec3(0, 0, 1))), deltaTime);
	}
	if (window->KeyHold(GLFW_KEY_S)) {
		GO->transform->Move(glm::normalize(glm::rotate(GO->transform->GetWorldRotation(), glm::vec3(0, 0, 1))), -deltaTime);
	}
}

void AnimationInput::OnKeyPress(int key, int mods)
{
	unsigned int animationID = 0;
	switch (key)
	{
		case GLFW_KEY_6: {
			if (GO->animation)
				GO->animation->SetAnimation(animationID);
			break;
		}
		case GLFW_KEY_7: {
			if (GO->animation)
				GO->animation->SetDefaultPose();
			break;
		}
		case GLFW_KEY_8: {
			auto SGO = Manager::GetPicker()->GetSelectedObject();
			if (SGO && dynamic_cast<SkinnedMesh*>(SGO->GetMesh())) {
				GO = SGO;
				cout << "[SUCCESS] - New Object attached to controller" << endl;
			}
			else {
				cout << "[ERROR] - Not a skinned objects" << endl;
			}
			break;
		}
		case GLFW_KEY_Y: {
			if (GO->animation)
				GO->animation->TogglePlayback();
			break;
		}
	}
}

void AnimationInput::OnKeyRelease(int key, int mods)
{
}

void AnimationInput::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
//	cout << mouseY << "-" << mouseX << endl;
}

void AnimationInput::OnEvent(EventType Event, void *data)
{
}
