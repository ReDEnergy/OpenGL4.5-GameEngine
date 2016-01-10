#include "pch.h"

#include "AnimationInput.h"

AnimationInput::AnimationInput(GameObject *obj)
	: ObjectInput(InputGroup::IG_GAMEPLAY), GO(obj)
{
	GO->transform->SetMoveSpeed(2);
	GO->transform->SetRotationSpeed(200);
	//((SkinnedMesh*)GO->mesh)->ScaleAnimationTime("Run", 1.5f);
	//((SkinnedMesh*)GO->mesh)->ScaleAnimationTime("Idle", 0.3f);
	//((SkinnedMesh*)GO->mesh)->ScaleAnimationTime("Attack", 0.5f);
	//((SkinnedMesh*)GO->mesh)->ScaleAnimationTime("Die", 1.5f);
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
		((SkinnedMesh*)GO->mesh)->SetAnimationState(nullptr);
	}
}

void AnimationInput::OnKeyPress(int key, int mods)
{
	switch (key)
	{
	case GLFW_KEY_1:
		((SkinnedMesh*)GO->mesh)->SetAnimationState("Still");
		break;
	case GLFW_KEY_2:
		((SkinnedMesh*)GO->mesh)->SetAnimationState("Idle");
		break;
	case GLFW_KEY_3:
		((SkinnedMesh*)GO->mesh)->SetAnimationState("Run");
		break;
	case GLFW_KEY_4:
		((SkinnedMesh*)GO->mesh)->SetAnimationState("Attack");
		break;
	case GLFW_KEY_5:
		((SkinnedMesh*)GO->mesh)->SetAnimationState("Die");
		break;
	case GLFW_KEY_6:
		((SkinnedMesh*)GO->mesh)->SetAnimationState("combinedAnim_0");
		break;
	case GLFW_KEY_7:
		((SkinnedMesh*)GO->mesh)->SetAnimationState(nullptr);
		break;
	case GLFW_KEY_W:
		((SkinnedMesh*)GO->mesh)->SetAnimationState("Walk");
		break;
	case GLFW_KEY_F:
		((SkinnedMesh*)GO->mesh)->SetAnimationState("Attack");
		break;
	case GLFW_KEY_T:
		((SkinnedMesh*)GO->mesh)->DrawControlSkeleton();
		break;
	case GLFW_KEY_Y:
		((SkinnedMesh*)GO->mesh)->ToggleAnimationPlayback();
		break;
	}
}

void AnimationInput::OnKeyRelease(int key, int mods)
{
	if (key == GLFW_KEY_W || key == GLFW_KEY_S) {
		((SkinnedMesh*)GO->mesh)->SetAnimationState("Idle");
	}
}

void AnimationInput::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
//	cout << mouseY << "-" << mouseX << endl;
}

void AnimationInput::OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods)
{
	//printf("pos: %d %d \n button: %d \n action: %d \n mods: %d \n", mouseX, mouseY, button, action, mods);
}

void AnimationInput::OnEvent(EventType Event, Object *data)
{
}
