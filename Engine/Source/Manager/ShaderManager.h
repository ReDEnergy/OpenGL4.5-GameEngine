#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include <include/dll_export.h>
#include <include/pugixml.h>

using namespace std;

class Shader;

class DLLExport ShaderManager
{
	protected:
		ShaderManager();
		~ShaderManager();

	public:
		Shader* GetShader(const char* name);
		void Load(const char *file);
		void Reload();

	private:
		Shader* LoadShader(vector<string> &shaderFiles);

		void ReadShader(pugi::xml_node &shaderXML);
		void ReloadFromFile();

	private:
		unordered_map<string, Shader*> shaders;

};