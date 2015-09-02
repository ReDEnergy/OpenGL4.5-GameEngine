#pragma once
#include <Engine.h>

class hkpCharacterRigidBody;
class hkpCharacterContext;

class Player: public GameObject
{
	public:
		Player(const GameObject &gameObject);
		~Player();

		// Player actions
		void Update(float deltaTime);
		void Jump();
		void MoveLeft(float deltaTime);
		void MoveRight(float deltaTime);
		void ResetMotion();

	private:
		hkpCharacterRigidBody *characterRigidBody;
		hkpCharacterContext *characterContext;

		bool wantToJump;
		float moveSpeed;
		float deltaMove;
};
