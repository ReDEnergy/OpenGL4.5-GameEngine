//#include <pch.h>
#include "EditorInput.h"

#include <Manager/Manager.h>
#include <Manager/RenderingSystem.h>
#include <UI/MenuSystem.h>

EditorInput::EditorInput()
	: ObjectInput(InputGroup::IG_EDITOR)
{
	SubscribeToEvent("ssao");
	SubscribeToEvent("forward-rendering");
	SubscribeToEvent("vsync-rendering");
}

void EditorInput::OnKeyPress(int key, int mods) {
}

void EditorInput::OnEvent(const char *eventID, Object *data) {
	ToggleEntry *entry = (ToggleEntry*)data;

	if (strcmp(eventID, "ssao") == 0) {
		Manager::RenderSys->Set(RenderState::SS_AO, entry->GetState());
	}
	if (strcmp(eventID, "forward-rendering") == 0) {
		Manager::RenderSys->Set(RenderState::POST_PROCESS, entry->GetState());
	}
	if (strcmp(eventID, "vsync-rendering") == 0) {
		Manager::RenderSys->Set(RenderState::VSYNC, entry->GetState());
		wglSwapIntervalEXT(entry->GetState());
	}
}