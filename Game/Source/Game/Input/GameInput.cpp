#include "GameInput.h"

#include <include/gl.h>

#include <Core/Engine.h>
#include <Core/Camera/Camera.h>
#include <Core/WindowObject.h>
#include <Core/InputSystem.h>

#include <Game/Game.h>
#include <Game/State/GameState.h>
#include <Manager/AudioManager.h>
#include <Manager/EventSystem.h>
#include <Manager/Manager.h>
#include <Event/EventType.h>


GameInput::GameInput(Game *game)
	: ObjectInput(InputGroup::IG_GAMEPLAY), game(game)
{

}

void GameInput::Update(float deltaTime, int mods) {

	if (InputSystem::KeyHold(GLFW_KEY_RIGHT))
		game->activeCamera->MoveRight(deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_UP))
		game->activeCamera->MoveForward(deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_LEFT))
		game->activeCamera->MoveRight(-deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_DOWN))
		game->activeCamera->MoveBackward(deltaTime);

	game->activeCamera->Update();
}

void GameInput::OnKeyPress(int key, int mods)
{
	switch (key)
	{
		case GLFW_KEY_ESCAPE:
			Manager::GetEvent()->EmitSync(EventType::OPEN_GAME_MENU, nullptr);
			return;

		case GLFW_KEY_F6:
			Manager::GetAudio()->PlaySoundFX("bell");
			return;

		case GLFW_KEY_F7:
			Manager::GetAudio()->PlayStream("relax");
			return;

	}
}

void GameInput::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
//	cout << mouseY << "-" << mouseX << endl;
}

void GameInput::OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods)
{
	//printf("pos: %d %d \n button: %d \n action: %d \n mods: %d \n", mouseX, mouseY, button, action, mods);
}