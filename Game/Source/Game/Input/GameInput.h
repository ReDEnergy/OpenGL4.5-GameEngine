#pragma once
#include <Component/ObjectInput.h>

using namespace std;
class Game;

class GameInput : public ObjectInput {
	public:
		GameInput(Game *game);
		void OnKeyPress(int key, int mods);
		void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods);

	private:
		Game			*game;
};