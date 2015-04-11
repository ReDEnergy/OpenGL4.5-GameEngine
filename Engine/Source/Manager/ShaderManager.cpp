//#include <pch.h>
#include "ShaderManager.h"

#include <string>
#include <vector>
#include <assert.h>

#include <include/gl.h>
#include <include/pugixml.h>
#include <include/utils.h>

#include <GPU/Shader.h>
#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>

static const string _PATH("Resources\\Shaders\\");

ShaderManager::ShaderManager() {
}

ShaderManager::~ShaderManager() {
	for (auto shader: shaders)
		SAFE_FREE(shader.second)
}

void ShaderManager::Load(const char *file) {
	Manager::Debug->InitManager("Shaders");

	pugi::xml_document *doc = pugi::LoadXML(file);
	pugi::xml_node shaders = doc->child("shaders");
	for (pugi::xml_node shaderXML: shaders.children()) {
		ReadShader(shaderXML);
	}
}

void ShaderManager::ReadShader(pugi::xml_node &shaderXML) {

	const char* name = shaderXML.child_value("name");

	if (GetShader(name))
		return;

	// const char* path = shaderXML.child_value("path");
	const char* compute = shaderXML.child_value("compute");
	const char* vertex = shaderXML.child_value("vertex");
	const char* fragment = shaderXML.child_value("fragment");
	const char* geometry = shaderXML.child_value("geometry");
	const char* tessEval = shaderXML.child_value("tess_eval");
	const char* tessCtrl = shaderXML.child_value("tess_control");

	// TODO - add support for path
	vector<string> shaderFiles;

	if (*compute) {
		shaderFiles.push_back(compute);
	} else {
		if (*vertex)	shaderFiles.push_back(vertex);
		if (*geometry)	shaderFiles.push_back(geometry);
		if (*tessCtrl)	shaderFiles.push_back(tessCtrl);
		if (*tessEval)	shaderFiles.push_back(tessEval);
		if (*fragment)	shaderFiles.push_back(fragment);
	}
	shaders[name] = LoadShader(shaderFiles);
}

Shader* ShaderManager::LoadShader(vector<string> &shaderFiles) {
	for (auto &file: shaderFiles) {
		file = _PATH + file + ".glsl";
	}
	Shader *shader = new Shader();
	shader->SetShaderFiles(shaderFiles);
	return shader;
}

Shader* ShaderManager::GetShader(const char* name) {
	if (shaders[name])
		return shaders[name];
	return nullptr;
}

void ShaderManager::Reload() {
	for (auto shader: shaders)
		shader.second->Reload();
}

bool ShaderManager::PushState(const Shader *shader)
{
	if (programs.empty() || programs.top() != shader) {
		shader->Use();
		programs.push(shader);
		return true;
	}

	return false;
}

const Shader* ShaderManager::PopState()
{
	if (programs.empty())
		return NULL;

	const Shader *sh = programs.top();
	programs.pop();
	return sh;
}

void ShaderManager::ReloadFromFile() {
	for (auto shader: shaders)
		shader.second->Reload();
}