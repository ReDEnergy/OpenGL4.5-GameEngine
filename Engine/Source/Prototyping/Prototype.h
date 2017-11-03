#pragma once
#include <include/dll_export.h>
#include <Event/EventListener.h>
#include <Component/ObjectInput.h>

class Camera;
class Shader;

///////////////////////////////////////////////////////////////////////////////
// THIS CLASS DEFINITION SERVES AS A TEMPLATE FOR PROTOTYPING
// INTENDED TO BE USED AS A BASE CLASS
// ONLY GENERAL FUNCTIONALITIES MAY BE ADDED HERE

class DLLExport Prototype :
	public ObjectInput,
	public EventListener
{
	public:
		Prototype() {};
		~Prototype() {};
};