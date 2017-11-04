#pragma once
#include <PCH/pch.h>

class GameObject;

class SimpleObjectControl : public ObjectInput {

	public:
		SimpleObjectControl(GameObject *obj);
		~SimpleObjectControl();

		void Update(float deltaTime, int mods);
		void OnKeyPress(int key, int mods);
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);

	public:
		GameObject *GO;
};