#pragma once
#include <include/dll_export.h>
#include <include/glm.h>
#include <string>

#include <Manager/RenderingSystem.h>

class WindowProperties;

class DLLExport ConfigFile
{
	protected:
		ConfigFile();
		~ConfigFile();

	public:
		void Load(const char *fileName);
		void ReadGraphicState(const char* propertyName, RenderState STATE);
		std::string GetResourceFileLoc(const char *resourceType);

	public:
		WindowProperties* windowProperties;
};

