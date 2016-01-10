#include "pch.h"

#include "AnimationInput.h"

AnimationInput::AnimationInput(GameObject *obj)
	: ObjectInput(InputGroup::IG_GAMEPLAY), GO(obj)
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

void AnimationInput::Update(float deltaTime, int mods)
{
	if (mods != GLFW_MOD_SHIFT)
		return;

	if (InputSystem::KeyHold(GLFW_KEY_D)) {
		GO->transform->RotateWorldOY(-deltaTime);
	}
	if (InputSystem::KeyHold(GLFW_KEY_A)) {
		GO->transform->RotateWorldOY(deltaTime);
	}
	if (InputSystem::KeyHold(GLFW_KEY_W)) {
		GO->transform->Move(glm::normalize(glm::rotate(GO->transform->GetWorldRotation(), glm::vec3(0, 0, 1))), deltaTime);
	}
	if (InputSystem::KeyHold(GLFW_KEY_S)) {
		GO->transform->Move(glm::normalize(glm::rotate(GO->transform->GetWorldRotation(), glm::vec3(0, 0, 1))), -deltaTime);
	}
	if (InputSystem::KeyHold(GLFW_KEY_P)) {
		GO->animation->SetAnimation(nullptr);
	}
}

void AnimationInput::OnKeyPress(int key, int mods)
{
	switch (key)
	{
		case GLFW_KEY_6: {
			GO->animation->SetAnimation("combinedAnim_0");
			break;
		}
		case GLFW_KEY_7: {
			GO->animation->SetAnimation(nullptr);
			break;
		}
		case GLFW_KEY_8: {
			auto SGO = Manager::GetPicker()->GetSelectedObject();
			if (SGO && dynamic_cast<SkinnedMesh*>(SGO->mesh)) {
				GO = SGO;
				cout << "[SUCCESS] - New Object attached to controller" << endl;
			}
			else {
				cout << "[ERROR] - Not a skinned objects" << endl;
			}
			break;
		}
		case GLFW_KEY_Y: {
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

void AnimationInput::OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods)
{
	//printf("pos: %d %d \n button: %d \n action: %d \n mods: %d \n", mouseX, mouseY, button, action, mods);
}

void AnimationInput::OnEvent(EventType Event, void *data)
{
}
