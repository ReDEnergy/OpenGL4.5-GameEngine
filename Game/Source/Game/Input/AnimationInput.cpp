#include "AnimationInput.h"

#include <include/gl.h>

#include <Component/SkinnedMesh.h>

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


AnimationInput::AnimationInput()
	: ObjectInput(InputGroup::IG_GAMEPLAY)
{

}

AnimationInput::~AnimationInput()
{

}

void AnimationInput::Update(float deltaTime, int mods)
{

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