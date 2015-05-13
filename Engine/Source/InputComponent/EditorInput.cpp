//#include <pch.h>
#include "EditorInput.h"

#include <Manager/Manager.h>
#include <Manager/RenderingSystem.h>

EditorInput::EditorInput()
	: ObjectInput(InputGroup::IG_EDITOR)
{
}

void EditorInput::OnKeyPress(int key, int mods) {
}