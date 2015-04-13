//#include <pch.h>
#include "GlobalInput.h"

#include <include/gl.h>
#include <include/glfw_keys.h>

GlobalInput::GlobalInput()
	: ObjectInput(InputGroup::IG_ENGINE)
{

}

void GlobalInput::OnKeyPress(int key, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		// OpenGame Menu
		return;
	}
}

void GlobalInput::OnEvent(const char* eventID, Object *data)
{

}
