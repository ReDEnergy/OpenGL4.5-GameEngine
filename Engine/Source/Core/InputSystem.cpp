//#include <pch.h>
#include "InputSystem.h"
#include <Core/WindowManager.h>
#include <Component/ObjectInput.h>

static const int MAX_KEYS = 384;

list<ObjectInput*> InputSystem::observers;
list<ObjectInput*> InputSystem::activeObserver;

bool* InputSystem::keyStates = new bool[MAX_KEYS];
int InputSystem::scancode = GLFW_KEY_UNKNOWN;
int InputSystem::mods = GLFW_KEY_UNKNOWN;
bool InputSystem::ruleChanged = false;

void InputSystem::KeyCallback(GLFWwindow *W, int key, int scancode, int action, int mods) {
	keyStates[key] = action ? true : false;
	InputSystem::scancode = scancode;
	InputSystem::mods = mods;
	NotifyObservers(key, action, mods);
}

bool InputSystem::KeyHold(int keyCode) {
	return keyStates[keyCode];
}

void InputSystem::ResetKeys() {
	for (int i=0; i<MAX_KEYS; i++)
		keyStates[i] = GLFW_RELEASE;
}

void InputSystem::CursorMove(GLFWwindow *W, double posX, double posY) {
	WindowObject *Wobj = WindowManager::GetWindowObject(W);

	glm::ivec2 mousePos = glm::ivec2(posX, posY);
	glm::ivec2 delta = mousePos - Wobj->pointerPos;
	NotifyObservers(mousePos.x, mousePos.y, delta.x, delta.y);
	Wobj->SetPointerPosition(mousePos);
}

void InputSystem::Subscribe(ObjectInput *IC) {
	observers.push_back(IC);
	if (IC->IsActive())
		activeObserver.push_back(IC);
}

void InputSystem::RemoveSubscription(ObjectInput *IC) {
	// TODO - a linked list is better for observer not vector
}

void InputSystem::RuleUpdate() {
	ruleChanged = true;
}

void InputSystem::EndFrame() {
	if (ruleChanged) {
		activeObserver.clear();
		for (auto obs:observers) {
			if (obs->IsActive())
				activeObserver.push_back(obs);
		}
		ruleChanged = false;
	}
}

void InputSystem::UpdateObservers(float deltaTime) {
	for (auto obs:activeObserver) {
		if (obs->IsActive())
			obs->Update(deltaTime, mods);
	}
}

void InputSystem::NotifyObservers(int key, int action, int mods) {
	if (action == GLFW_PRESS) {
		for (auto obs:activeObserver)
			if (obs->IsActive())
				obs->OnKeyPress(key, mods);
		return;
	}
	if (action == GLFW_RELEASE) {
		for (auto obs:activeObserver)
			if (obs->IsActive())
				obs->OnKeyRelease(key, mods);
	}
}

void InputSystem::NotifyObservers(int mouseX, int mouseY, int deltaX, int deltaY) {
	for (auto obs:activeObserver)
		obs->OnMouseMove(mouseX, mouseY, deltaX, deltaY);
}