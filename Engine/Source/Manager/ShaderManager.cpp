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
	for (auto &group : shaderGroups)
	{
		for (auto &shader : group.second.shaders)
		{
			SAFE_FREE(shader.second)
		}
	}
}

void ShaderManager::LoadConfigFile(const char *file)
{
	pugi::xml_document *doc = pugi::LoadXML(file);
	if (doc)
	{
		auto shaderNamespace = doc->child_value("namespace");

		auto &group = shaderGroups[shaderNamespace];
		group.basePath = doc->child_value("base-path");
		group.shaderNamespace = shaderNamespace;

		pugi::xml_node shaders = doc->child("shaders");
		for (pugi::xml_node shaderXML : shaders.children()) {
			ReadShader(shaderXML, group);
		}
	}
}

void ShaderManager::SetConfigFiles(std::vector<std::string> configFiles)
{
	shaderDataFiles = configFiles;
	Reload();
}

void ShaderManager::ReadShader(pugi::xml_node &shaderXML, ShadersGroup &group)
{
	const char* name = shaderXML.attribute("name").as_string();

	Shader *shader;
	auto it = group.shaders.find(name);
	if (it != group.shaders.end()) {
		shader = it->second;
		shader->ClearShaders();
	}
	else
	{
		shader = new Shader(*name);
	}

	for (pugi::xml_node shaderNode : shaderXML.children()) {
		auto fileName = shaderNode.text().get();
		auto file = group.basePath + fileName + ".glsl";
		auto type = shaderType[shaderNode.name()];
		shader->AddShader(file, type);
	}

	shader->CreateAndLink();
	group.shaders[name] = shader;
}

Shader* ShaderManager::GetShader(std::string name, std::string shaderNameSpace)
{
	auto it = shaderGroups.find(shaderNameSpace);
	if (it != shaderGroups.end()) {
		auto it2 = it->second.shaders.find(name);
		if (it2 != it->second.shaders.end()) {
			return it2->second;
		}
	}

	return nullptr;
}

void ShaderManager::Reload()
{
	for (auto &file : shaderDataFiles) {
		LoadConfigFile(file.c_str());
	}
}

void ShaderManager::Reload(std::string shaderNamespace)
{
	auto it = shaderGroups.find(shaderNamespace);
	if (it != shaderGroups.end()) {
		auto config = it->second;
		for (auto shader : config.shaders)
		{
			shader.second->Reload();
		}
	}
}
