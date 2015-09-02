#include "pch.h"

#include "GameInput.h"

#include <Game/Game.h>

GameInput::GameInput(Game *game)
	: ObjectInput(InputGroup::IG_GAMEPLAY), game(game)
{

}

void GameInput::Update(float deltaTime, int mods) {

	Camera *activeCamera = Manager::GetScene()->GetActiveCamera();

	if (InputSystem::KeyHold(GLFW_KEY_RIGHT))
		activeCamera->MoveRight(deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_UP))
		activeCamera->MoveForward(deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_LEFT))
		activeCamera->MoveRight(-deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_DOWN))
		activeCamera->MoveBackward(deltaTime);

	activeCamera->Update();
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
			//Manager::GetAudio()->PlayStream("relax");
			return;
	}

	if (mods == (GLFW_MOD_SHIFT + GLFW_MOD_CONTROL)) {
		if (key == GLFW_KEY_B) {
			Manager::GetEvent()->EmitSync("barrels-light", nullptr);
		}
		if (key == GLFW_KEY_N) {
			Manager::GetEvent()->EmitSync("barrels", nullptr);
		}
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