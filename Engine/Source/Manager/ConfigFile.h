#pragma once
#include <include/dll_export.h>
#include <include/glm.h>

#include <Manager/RenderingSystem.h>
#include <Core/WindowObject.h>

class DLLExport ConfigFile
{
	protected:
		ConfigFile();
		~ConfigFile();

	public:
		void Load(const char *fileName);
		void ReadGraphicState(char * propertyName, RenderState STATE);
		const char* GetResourceFileLoc(const char *resourceType);

	public:
		WindowProperties windowProperties;
};

