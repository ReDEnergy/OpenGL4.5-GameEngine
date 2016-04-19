#include <pch.h>
#include "GUI.h"

#include <templates/singleton.h>

unordered_map<QT_INSTACE, void*> GUI::instances;

void GUI::Init()
{

}

void GUI::Set(QT_INSTACE key, void * instace)
{
	instances[key] = instace;
}