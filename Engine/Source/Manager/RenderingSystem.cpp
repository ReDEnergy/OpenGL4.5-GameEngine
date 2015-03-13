//#include <pch.h>
#include "RenderingSystem.h"

#include <include/gl.h>

//bool* RenderingSystem::states = nullptr;
//bool* RenderingSystem::prevStates = nullptr;
//int RenderingSystem::debugParam = 0;

void RenderingSystem::Init() {
	debugParam = 0;
	states = new bool[10]();
	prevStates = new bool[10]();
	Set(RenderState::POST_PROCESS, true);
	Set(RenderState::SHADOW, true);
	Set(RenderState::SS_AO, false);
}

bool RenderingSystem::Is(RenderState STATE) {
	return states[STATE] == true;
}

void RenderingSystem::Set(RenderState STATE, bool value) {
	SavePreviousState(STATE);
	states[STATE] = value;
	if (STATE == RenderState::WIREFRAME)
		value ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	UpdateGlobalState();
}

void RenderingSystem::Revert(RenderState STATE) {
	bool value = prevStates[STATE];
	Set(STATE, value);
}

bool RenderingSystem::Toggle(RenderState STATE) {
	SavePreviousState(STATE);
	states[STATE] = !states[STATE];
	UpdateGlobalState();
	return states[STATE];
}

void RenderingSystem::UpdateGlobalState() {
	states[RenderState::FORWARD] = false;
	if (states[RenderState::WIREFRAME] || !states[RenderState::POST_PROCESS]) {
		SavePreviousState(RenderState::FORWARD);
		states[RenderState::FORWARD] = true;
	}
}

void RenderingSystem::SavePreviousState(RenderState STATE) {
	prevStates[STATE] = states[STATE];
}