//#include <pch.h>
#include "DebugInput.h"

#include <include/glfw_keys.h>

#include <Core/Engine.h>
#include <Core/InputSystem.h>
#include <GPU/Shader.h>

#include <Manager/AudioManager.h>
#include <Manager/DebugInfo.h>
#include <Manager/EventSystem.h>
#include <Manager/Manager.h>
#include <Manager/RenderingSystem.h>
#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>

#include <UI/MenuSystem.h>

DebugInput::DebugInput()
	: ObjectInput(InputGroup::IG_DEBUG)
{
	SubscribeToEvent("ssao");
	SubscribeToEvent("forward-rendering");
}

void DebugInput::OnKeyPress(int key, int mods) {
	if (mods == GLFW_KEYMOD_CTRL_SHIFT) {
		switch (key)
		{
			case GLFW_KEY_C:
				Manager::Event->EmitSync(EventType::SWITCH_CAMERA, nullptr);
				return;

			case GLFW_KEY_B:
				Manager::Event->EmitSync(EventType::DEBUG_BARREL_SPAWN, nullptr);
				return;

			case GLFW_KEY_I:
				printf("\n\nActive objects: %d\n\n", Manager::Scene->activeObjects.size());
				return;

			case GLFW_KEY_V:
				Manager::Debug->debugView = !Manager::Debug->debugView;
				return;
		}

		{
			int val = key - GLFW_KEY_F1;
			if (val >= 0 && val < 12) {
				Manager::RenderSys->debugParam = val + 1;
				return;
			}
		}

		if (key == GLFW_KEY_S) {
			Manager::RenderSys->Toggle(RenderState::SS_AO);
			return;
		}
	}

	switch (key)
	{
	case GLFW_KEY_P: {
		Engine::Pause();
		return;
	}

	case GLFW_KEY_F1: {
		Manager::RenderSys->Toggle(RenderState::DEBUG);
		return;
	}

	case GLFW_KEY_F3: {
		Manager::RenderSys->Toggle(RenderState::WIREFRAME);
		return;
	}

	case GLFW_KEY_F4: {
		Manager::RenderSys->Toggle(RenderState::POST_PROCESS);
		return;
	}

	case GLFW_KEY_F5: {
		Manager::Shader->Reload();
		return;
	}
	}
}

void DebugInput::OnEvent(const char *eventID, Object *data) {
	if (strcmp(eventID, "ssao") == 0) {
		ToggleEntry *entry = (ToggleEntry*) data;
		Manager::RenderSys->Set(RenderState::SS_AO, entry->GetState());
	}
	if (strcmp(eventID, "forward-rendering") == 0) {
		ToggleEntry *entry = (ToggleEntry*) data;
		Manager::RenderSys->Set(RenderState::POST_PROCESS, entry->GetState());
	}
}