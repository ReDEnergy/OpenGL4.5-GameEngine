//#include <pch.h>
#include "ObjectInput.h"

#include <Core/InputSystem.h>

ObjectInput::ObjectInput(InputGroup GROUP) {
	this->GROUP = GROUP;
	InputSystem::Subscribe(this);
}

ObjectInput::~ObjectInput() {
}

bool ObjectInput::IsActive() {
	return InputRules::IsActiveRule(this->GROUP);
}