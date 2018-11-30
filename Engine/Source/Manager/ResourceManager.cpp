#include "ResourceManager.h"

#include <iostream>

#include <include/glm.h>
#include <include/glm_utils.h>
#include <include/utils.h>
#include <sstream>

#include <Config/ResourcePath.h>

#include <Core/GameObject.h>
#include <Component/Mesh.h>
#include <Component/SkinnedMesh.h>
#include <Component/Transform/Transform.h>
#include <Component/Renderer.h>

#include <Manager/Manager.h>
#include <Manager/AudioManager.h>
#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>
#include <Manager/DebugInfo.h>
#include <Utils/Serialization.h>

#ifdef PHYSICS_ENGINE
#include <Component/Physics.h>
#endif

using namespace std;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	for (auto &obj : _objects)
		SAFE_FREE(obj.second);
	for (auto &obj : _meshes)
		SAFE_FREE(obj.second);
}

void ResourceManager::Load(const char* file)
{
	// Load document
	const pugi::xml_document &doc = *(pugi::LoadXML(file));

	LoadMeshes(doc);
	LoadPrefabObjects(doc);
	LoadGameAudio(doc);
}

void ResourceManager::LoadMeshes(const pugi::xml_document &doc)
{
	const char* meshName;
	pugi::xml_node meshesXML = doc.child("meshes");

	for (pugi::xml_node mesh : meshesXML.children())
	{
		bool skinned = mesh.attribute("skinned").as_bool();
		bool noMaterial = mesh.attribute("material") ? true : false;

		meshName = mesh.child_value("name");

		Mesh *M = skinned ? new SkinnedMesh(meshName) : new Mesh(meshName);

		if (noMaterial)		M->UseMaterials(false);

		if (!M->LoadMesh(RESOURCE_PATH::MODELS + mesh.child_value("path"), mesh.child_value("file"))) {
			SAFE_FREE(M);
			continue;
		}

		// Create gameobject for this mesh
		GameObject *GO = new GameObject(meshName);
		GO->SetMesh(M);

		// Save Mesh and GameObject references
		_meshes[meshName] = M;
		_objects[meshName] = GO;
	}
}

void ResourceManager::LoadGameAudio(const pugi::xml_document &doc)
{
	const char* audioName;
	pugi::xml_node audiosXML = doc.child("audio");

	for (pugi::xml_node audio : audiosXML.children())
	{
		cout << "Audio Type: " << audio.name() << endl;

		audioName = audio.child_value("name");
		string fileLocation = RESOURCE_PATH::AUDIO + audio.child_value("path") + "/" + audio.child_value("file");

		if (strcmp(audio.name(), "music") == 0)
		{
			Manager::Audio->LoadAudio(fileLocation, audioName, AUDIO_TYPE::MUSIC);
		}

		if (strcmp(audio.name(), "soundfx") == 0)
		{
			Manager::Audio->LoadAudio(fileLocation, audioName, AUDIO_TYPE::SOUND_FX_FILE);

			pugi::xml_node effectsXML = audio.child("effects");

			for (pugi::xml_node fx : effectsXML.children())
			{
				const char* effectName = fx.child_value("name");
				float offset = fx.child("start").text().as_float();
				float length = fx.child("end").text().as_float() - offset;
				Manager::Audio->InitSoundFX(audioName, effectName, offset, length);
			}

		}
	}
}

void ResourceManager::LoadPrefabObjects(const pugi::xml_document &doc)
{
	const char *objName;
	const char* meshName;
	pugi::xml_node objects = doc.child("objects");
	unordered_map<string, OpenGL::CULL> CullMap;
	CullMap["front"] = OpenGL::CULL::FRONT;
	CullMap["back"] = OpenGL::CULL::BACK;
	CullMap["none"] = OpenGL::CULL::NONE;
	CullMap["both"] = OpenGL::CULL::BOTH;

	for (pugi::xml_node obj : objects.children())
	{
		objName = obj.child_value("name");
		meshName = obj.child_value("mesh");
		//auto shaderInfo = obj.child("shader");
		auto transformInfo = obj.child("transform");
		auto renderingInfo = obj.child("rendering");

		GameObject *GO = new GameObject(objName);
		Serialization::ReadTransform(transformInfo, *GO->transform);
		if (meshName) {
			GO->SetMesh(_meshes[meshName]);
		}

		//if (shaderInfo) {
		//	GO->UseShader(Manager::Shader->GetShader(shaderInfo.text().get()));
		//}

		if (renderingInfo) {
			auto shadowInfo = renderingInfo.child("shadow");
			if (shadowInfo)
				GO->renderer->SetCastShadow(shadowInfo.text().as_bool());
			auto cullingInfo = renderingInfo.child("culling");
			if (cullingInfo) {
				auto it = CullMap.find(cullingInfo.text().get());
				if (it != CullMap.end()) {
					GO->renderer->SetCulling(it->second);
				}
			}
		}

		_objects[objName] = GO;
	}
}

GameObject* ResourceManager::GetGameObject(const char *name) const
{
	GameObject* obj = nullptr;
	if (_objects.find(name) != _objects.end()) {
		obj = new GameObject(*_objects.at(name));
	}
	return obj;
}

GameObject* ResourceManager::GetPropObject(const char * name) const
{
	if (_objects.find(name) != _objects.end())
		return _objects.at(name);
	return nullptr;
}

Mesh* ResourceManager::GetMesh(const char *name) const
{
	if (_meshes.find(name) != _meshes.end()) {
		return _meshes.at(name);
	}
	return nullptr;
}

unsigned int ResourceManager::GetGameObjectUID(const char *name)
{
	if (name == nullptr)
		return -1;

	if (_counter.find(name) == _counter.end()) {
		_counter[name] = -1;
	}
	_counter[name]++;

	return _counter[name];
}

const unordered_map<string, Mesh*>& ResourceManager::GetMeshList() const
{
	return _meshes;
}
