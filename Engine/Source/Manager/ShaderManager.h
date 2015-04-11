#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <stack>

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
		void Load(const char *file);
		void Reload();
		bool PushState(const Shader *shader);
		const Shader* PopState();
		Shader* GetShader(const char* name);

	private:
		Shader* LoadShader(vector<string> &shaderFiles);

		void ReadShader(pugi::xml_node &shaderXML);
		void ReloadFromFile();

	private:
		unordered_map<string, Shader*> shaders;
		stack <const Shader*> programs;

};