#include "GameInput.h"

#include <include/gl.h>
#include <include/glfw_keys.h>

#include <Game/Game.h>
#include <Game/State/GameState.h>


GameInput::GameInput(Game *game)
	: ObjectInput(InputGroup::IG_GAMEPLAY), game(game)
{

}

void GameInput::OnKeyPress(int key, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (RuntimeState::STATE == RunState::GAMEPLAY) {
			RuntimeState::STATE = RunState::PAUSE_MENU;
			InputRules::SetRule(InputRule::R_IN_GAME_MENU);
		}
		return;
	}
}
