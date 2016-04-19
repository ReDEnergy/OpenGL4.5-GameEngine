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

using namespace std;

ShaderManager::ShaderManager()
{
	shaderType["compute"] = GL_COMPUTE_SHADER;
	shaderType["vertex"] = GL_VERTEX_SHADER;
	shaderType["fragment"] = GL_FRAGMENT_SHADER;

#ifndef OPENGL_ES
	shaderType["geometry"] = GL_GEOMETRY_SHADER;
	shaderType["tess_eval"] = GL_TESS_EVALUATION_SHADER;
	shaderType["tess_control"] = GL_TESS_CONTROL_SHADER;
#endif

}

ShaderManager::~ShaderManager()
{
	for (auto &shader: shaders)
		SAFE_FREE(shader.second)
}

void ShaderManager::Load(const char *file)
{
	shaderDataFile = file;
	Manager::Debug->InitManager("Shaders");

	pugi::xml_document *doc = pugi::LoadXML(file);
	if (doc)
	{
		BasePath = doc->child_value("base-path");

		pugi::xml_node shaders = doc->child("shaders");
		for (pugi::xml_node shaderXML : shaders.children()) {
			ReadShader(shaderXML);
		}
	}
}

void ShaderManager::ReadShader(pugi::xml_node &shaderXML)
{
	const char* name = shaderXML.attribute("name").as_string();

	// TODO - add support for path
	// const char* path = shaderXML.attribute("path").as_string();;

	Shader *shader;
	if (shaders.find(name) != end(shaders)) {
		shader = shaders.at(name);
		shader->ClearShaders();
	}
	else {
		shader = new Shader(*name);
	}

	for (pugi::xml_node shaderNode : shaderXML.children()) {
		auto fileName = shaderNode.text().get();
		auto file = BasePath + fileName + ".glsl";
		auto type = shaderType[shaderNode.name()];
		shader->AddShader(file, type);
	}

	auto res = shader->CreateAndLink();
	if (res) {
		shaders[name] = shader;
	}
}

Shader* ShaderManager::GetShader(const char* name)
{
	if (shaders.find(name) != shaders.end())
		return shaders.at(name);

	return nullptr;
}

void ShaderManager::Reload()
{
	Load(shaderDataFile.c_str());
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
	for (auto &shader: shaders)
		shader.second->Reload();
}