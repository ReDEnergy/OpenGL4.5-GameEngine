#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <stack>

#include <include/dll_export.h>
#include <include/utils.h>
#include <include/pugixml.h>

class Shader;

class DLLExport ShaderManager
{
	protected:
		ShaderManager();
		~ShaderManager();

	public:
		void Load(const char *file);
		void Reload();
		const Shader* PopState();
		Shader* GetShader(const char* name);

	private:
		void ReadShader(pugi::xml_node &shaderXML);
		void ReloadFromFile();

	private:
		std::string BasePath;
		std::unordered_map<std::string, uint> shaderType;
		std::unordered_map<std::string, Shader*> shaders;
		std::stack <const Shader*> programs;
		std::string shaderDataFile;

};