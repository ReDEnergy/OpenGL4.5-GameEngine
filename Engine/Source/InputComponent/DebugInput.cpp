#include "DebugInput.h"

#include <include/gl.h>

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
{
}

void DebugInput::OnKeyPress(int key, int mods) {
	if (mods == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT)) {
		switch (key)
		{
			case GLFW_KEY_C:
				Manager::GetEvent()->EmitAsync(EventType::SWITCH_CAMERA, nullptr);
				return;

			case GLFW_KEY_I:
				printf("\n\nActive objects: %u\n\n", static_cast<unsigned int>(Manager::Scene->GetSceneObjects().size()));
				return;

			case GLFW_KEY_V:
				Manager::GetDebug()->Toggle();
				return;
		}
	}

	if (key == GLFW_KEY_F5)
	{
		Manager::GetShader()->Reload();
		return;
	}
}
