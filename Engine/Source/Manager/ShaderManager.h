#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <stack>

#include <include/dll_export.h>
#include <include/utils.h>
#include <include/pugixml.h>

using namespace std;

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
		unordered_map<string, uint> shaderType;
		unordered_map<string, Shader*> shaders;
		stack <const Shader*> programs;
		const char *shaderCfgFile;

};