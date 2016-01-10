//#include <pch.h>
#include "DebugInput.h"

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
}

void DebugInput::OnKeyPress(int key, int mods) {
	if (mods == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT)) {
		switch (key)
		{
			case GLFW_KEY_C:
				Manager::Event->EmitAsync(EventType::SWITCH_CAMERA, nullptr);
				return;

			case GLFW_KEY_I:
				printf("\n\nActive objects: %d\n\n", Manager::Scene->GetActiveObjects().size());
				return;

			case GLFW_KEY_V:
				Manager::Debug->Toggle();
				return;
		}
	}

	if (key == GLFW_KEY_F5)
	{
		Manager::Shader->Reload();
		return;
	}
}