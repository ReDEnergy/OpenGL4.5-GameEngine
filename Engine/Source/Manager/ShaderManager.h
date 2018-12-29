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

		struct ShadersGroup
		{
			std::string shaderNamespace;
			std::string basePath;
			std::unordered_map<std::string, Shader*> shaders;
		};

	public:
		void AddConfigFile(std::string configFile);
		void SetConfigFiles(std::vector<std::string> configFiles);
		void Reload();
		void Reload(std::string shaderNamespace);

		Shader* GetShader(std::string name, std::string shaderNameSpace = "Engine");

	private:
		void LoadConfigFile(const char *file);
		void ReadShader(pugi::xml_node &shaderXML, ShadersGroup &group);

	private:
		std::vector<std::string> shaderDataFiles;
		std::unordered_map<std::string, uint> shaderType;

		std::unordered_map<std::string, ShadersGroup> shaderGroups;
};