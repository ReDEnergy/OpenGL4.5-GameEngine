//#include <pch.h>
#include "EditorInput.h"

#include <Manager/Manager.h>
#include <Manager/RenderingSystem.h>
#include <include/glfw_keys.h>

EditorInput::EditorInput()
	: ObjectInput(InputGroup::IG_EDITOR)
{
}

void EditorInput::OnKeyPress(int key, int mods) {
	if (mods == GLFW_KEYMOD_CTRL_SHIFT) {
	}
}