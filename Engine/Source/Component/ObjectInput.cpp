#include "ObjectInput.h"

#include <Core/InputSystem.h>
#include <Core/WindowManager.h>
#include <Core/WindowObject.h>

ObjectInput::ObjectInput()
{
	window = nullptr;
}

ObjectInput::~ObjectInput()
{
}

void ObjectInput::AttachTo(const char * windowName)
{
	window = WindowManager::GetWindowObjectByName(windowName);
	window->Subscribe(this);
}

void ObjectInput::AttachTo(WindowObject *window)
{
	this->window = window;
	window->Subscribe(this);
}
