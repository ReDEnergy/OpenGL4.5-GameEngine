#include "AnimationInput.h"

#include <include/gl.h>

#include <Component/SkinnedMesh.h>
#include <Component/Transform.h>

#include <Core/Engine.h>
#include <Core/GameObject.h>
#include <Core/Camera/Camera.h>
#include <Core/WindowObject.h>
#include <Core/InputSystem.h>

#include <Game/Game.h>
#include <Game/State/GameState.h>
#include <Manager/AudioManager.h>
#include <Manager/EventSystem.h>
#include <Manager/Manager.h>
#include <Event/EventType.h>

AnimationInput::AnimationInput(GameObject *obj)
	: ObjectInput(InputGroup::IG_GAMEPLAY), GO(obj)
{
	GO->transform->moveSpeed *= 1.5;
	((SkinnedMesh*)GO->mesh)->ScaleAnimationTime("Run", 1.5f);
	((SkinnedMesh*)GO->mesh)->ScaleAnimationTime("Idle", 0.3f);
	((SkinnedMesh*)GO->mesh)->ScaleAnimationTime("Attack", 0.5f);
	((SkinnedMesh*)GO->mesh)->ScaleAnimationTime("Die", 1.5f);
}

AnimationInput::~AnimationInput()
{

}

void AnimationInput::Update(float deltaTime, int mods)
{
	if (InputSystem::KeyHold(GLFW_KEY_D)) {
		GO->transform->RotateYaw(deltaTime);
	}
	if (InputSystem::KeyHold(GLFW_KEY_A)) {
		GO->transform->RotateYaw(-deltaTime);
	}
	if (InputSystem::KeyHold(GLFW_KEY_W)) {
		GO->transform->Move(glm::normalize(glm::rotate(GO->transform->rotationQ, glm::vec3(0, 0, 1))), deltaTime);
		((SkinnedMesh*)GO->mesh)->SetAnimationState("Run");
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
	case GLFW_KEY_F:
		((SkinnedMesh*)GO->mesh)->SetAnimationState("Attack");
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
