#pragma once
#include <PCH/pch.h>

using namespace std;
class GameObject;

class SimpleObjectControl : public ObjectInput {

	public:
		SimpleObjectControl(GameObject *obj);
		~SimpleObjectControl();

		void Update(float deltaTime, int mods);
		void OnKeyPress(int key, int mods);
		//void OnKeyRelease(int key, int mods);
		//void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
		//void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods);

	public:
		GameObject *GO;
};