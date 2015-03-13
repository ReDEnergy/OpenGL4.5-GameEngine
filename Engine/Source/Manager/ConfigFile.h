#pragma once
#include <include/dll_export.h>
#include <include/glm.h>

class DLLExport ConfigFile
{
	protected:
		ConfigFile();
		~ConfigFile();

	public:
		void Load(const char *fileName);
		const char* GetResourceFileLoc(const char *resourceType);

	public:
		glm::ivec2 resolution;
		glm::ivec2 position;
};

