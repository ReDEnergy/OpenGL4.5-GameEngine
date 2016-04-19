#include "GUIEntity.h"

GUIEntity::GUIEntity(const char * name)
{
	if (name)
		name = name;
}

void GUIEntity::SetName(const char * name)
{
	this->name = name;
}

const char * GUIEntity::GetName() const
{
	return name.c_str();
}
