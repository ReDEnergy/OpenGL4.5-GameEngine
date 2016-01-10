#pragma once

#include <Component/ObjectInput.h>
using namespace std;

/*
 * Class PlayerInput
 */

class Player;

class PlayerInput : public ObjectInput {
	public:
		PlayerInput(Player *player);
		void Update(float deltaTime, int mods);
		void OnKeyPress(int key, int action);
		void OnKeyRelease(int key, int action);
	private:
		Player *player;
};

