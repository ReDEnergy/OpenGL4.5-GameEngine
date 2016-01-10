#pragma once
#include <PCH/pch.h>

using namespace std;
class Game;

class GameInput : public ObjectInput {
	public:
		GameInput(Game *game);
		void Update(float deltaTime, int mods);
		void OnKeyPress(int key, int mods);
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
		void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods);

	private:
		Game *game;
};