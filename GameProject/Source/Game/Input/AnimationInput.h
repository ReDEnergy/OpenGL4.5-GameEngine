#pragma once
#include <PCH/pch.h>
using namespace std;

class AnimationInput : public ObjectInput, EventListener {

	public:
		AnimationInput(GameObject *obj);
		~AnimationInput();

		void Update(float deltaTime, int mods);
		void OnKeyPress(int key, int mods);
		void OnKeyRelease(int key, int mods);
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
		void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods);
		void OnEvent(EventType Event, void *data);

	public:
		GameObject *GO;
};