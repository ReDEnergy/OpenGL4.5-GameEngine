#include "PlayerInput.h"

#include <Component/Transform.h>
#include <Component/ObjectInput.h>
#include <Core/InputSystem.h>

#include <Game/Actors/Player.h>

#include <Manager/Manager.h>

//#include <Manager/HavokCore.h>

PlayerInput::PlayerInput(Player *player)
	: ObjectInput(InputGroup::IG_GAMEPLAY)
{
	this->player = player;
}

void PlayerInput::Update(float deltaTime, int mods) {
//	Manager::GetHavok()->MarkForWrite();
	{
		if (InputSystem::KeyHold(GLFW_KEY_LEFT))
			player->MoveLeft(deltaTime);

		if (InputSystem::KeyHold(GLFW_KEY_RIGHT))
			player->MoveRight(deltaTime);
	}
//	Manager::GetHavok()->UnmarkForWrite();
}

void PlayerInput::OnKeyPress(int key, int mods) {
//	Manager::GetHavok()->MarkForWrite();
	{
		if (InputSystem::KeyHold(GLFW_KEY_LEFT))
			player->transform->SetRotation(glm::vec3(0, 180, 0));

		if (InputSystem::KeyHold(GLFW_KEY_RIGHT))
			player->transform->SetRotation(glm::vec3(0, 0, 0));

		if (key == GLFW_KEY_SPACE)
			player->Jump();
	}
//	Manager::GetHavok()->UnmarkForWrite();
}

void PlayerInput::OnKeyRelease(int key, int mods) {
}