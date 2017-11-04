#pragma once
#include <PCH/pch.h>

class AnimationInput : public ObjectInput, EventListener {

	public:
		AnimationInput(GameObject *obj);
		~AnimationInput();

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods);
		void OnKeyRelease(int key, int mods);
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
		void OnEvent(EventType Event, void *data);

	public:
		GameObject *GO;
};