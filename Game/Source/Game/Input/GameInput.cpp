#include "GameInput.h"

#include <include/gl.h>
#include <include/glfw_keys.h>

#include <Core/Engine.h>
#include <Core/WindowObject.h>
#include <Core/InputSystem.h>

#include <Game/Game.h>
#include <Game/State/GameState.h>
#include <Manager/EventSystem.h>
#include <Manager/Manager.h>
#include <Event/EventType.h>


GameInput::GameInput(Game *game)
	: ObjectInput(InputGroup::IG_GAMEPLAY), game(game)
{

}

void GameInput::OnKeyPress(int key, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		Manager::GetEvent()->EmitSync(EventType::OPEN_GAME_MENU, nullptr);
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