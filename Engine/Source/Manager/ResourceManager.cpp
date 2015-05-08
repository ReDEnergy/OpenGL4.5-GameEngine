//#include <pch.h>
#include "ResourceManager.h"

#include <include/glm.h>
#include <include/glm_utils.h>
#include <include/utils.h>
#include <sstream>

#include <Manager/Manager.h>
#include <Manager/AudioManager.h>
#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>
#include <Manager/DebugInfo.h>

#include <Core/GameObject.h>
#include <Component/Mesh.h>
#include <Component/SkinnedMesh.h>
#include <Component/Transform.h>
#include <Component/Renderer.h>

#ifdef PHYSICS_ENGINE
#include <include/havok.h>
#include <Component/Physics.h>
#endif

ResourceManager::ResourceManager() {
}

ResourceManager::~ResourceManager() {
	for (auto obj : _objects)
		SAFE_FREE(obj.second);
	for (auto obj : meshes)
		SAFE_FREE(obj.second);
}

void ResourceManager::Load(const char* file) {
	Manager::Debug->InitManager("Resources");

	// Load document
	const pugi::xml_document &doc = *(pugi::LoadXML(file));

	LoadMeshes(doc);
	LoadGameObjects(doc);
	LoadGameAudio(doc);

}

void ResourceManager::LoadMeshes(const pugi::xml_document &doc)
{
	const char* meshName;
	pugi::xml_node meshesXML = doc.child("meshes");

	for (pugi::xml_node mesh : meshesXML.children()) {
		bool skinned = mesh.attribute("skinned").as_bool();
		bool quad = mesh.attribute("quad").as_bool();
		bool noMaterial = mesh.attribute("material") ? true : false;

		meshName = mesh.child_value("name");

		Mesh *M = skinned ? new SkinnedMesh(meshName) : new Mesh(meshName);

		if (quad) {
			M->SetGlPrimitive(GL_QUADS);
		}
		if (noMaterial) {
			M->UseMaterials(false);
		}
		if (!M->LoadMesh(RESOURCE_PATH::MODELS + mesh.child_value("path"), mesh.child_value("file"))) {
			SAFE_FREE(M);
			continue;
		}
		meshes[meshName] = M;
	}
}

void ResourceManager::LoadGameAudio(const pugi::xml_document &doc)
{
	const char* audioName;
	pugi::xml_node audiosXML = doc.child("audio");

	for (pugi::xml_node audio : audiosXML.children()) {
		cout << "Audio Type: " << audio.name() << endl;

		audioName = audio.child_value("name");
		string fileLocation = RESOURCE_PATH::AUDIO + audio.child_value("path") + '\\' + audio.child_value("file");

		if (strcmp(audio.name(), "music") == 0) {
			Manager::Audio->LoadAudio(fileLocation, audioName, AUDIO_TYPE::MUSIC);
		}

		if (strcmp(audio.name(), "soundfx") == 0) {
			Manager::Audio->LoadAudio(fileLocation, audioName, AUDIO_TYPE::SOUND_FX_FILE);

			pugi::xml_node effectsXML = audio.child("effects");

			for (pugi::xml_node fx : effectsXML.children()) {
				const char* effectName = fx.child_value("name");
				float offset = fx.child("start").text().as_float();
				float length = fx.child("end").text().as_float() - offset;
				Manager::Audio->InitSoundFX(audioName, effectName, offset, length);
			}

		}
	}
}

void ResourceManager::LoadGameObjects(const pugi::xml_document &doc)
{
	const char *objName;
	const char* meshName;
	pugi::xml_node shaderInfo;
	pugi::xml_node physicsInfo;
	pugi::xml_node transformInfo;
	pugi::xml_node renderingInfo;
	pugi::xml_node objects = doc.child("objects");

	for (pugi::xml_node obj : objects.children()) {
		objName = obj.child_value("name");
		meshName = obj.child_value("mesh");
		shaderInfo = obj.child("shader");
		physicsInfo = obj.child("physics");
		transformInfo = obj.child("transform");
		renderingInfo = obj.child("shadow");

		GameObject *GO = new GameObject(objName);
		SetTransform(transformInfo, *GO->transform);
		if (meshName) {
			GO->mesh = meshes[meshName];
			GO->SetupAABB();
		}

		if (shaderInfo) {
			GO->UseShader(Manager::Shader->GetShader(shaderInfo.text().get()));
		}

		#ifdef PHYSICS_ENGINE
		if (physicsInfo) {
			GO->physics = new Physics(GO);
			GO->physics->LoadHavokFile(physicsInfo.child_value("file"));
		}
		#endif

		if (renderingInfo) {
			GO->renderer->SetCastShadow(true);
		}

		_objects[objName] = GO;
	}
}

GameObject* ResourceManager::GetGameObject(const char *name) {
	if (_objects[name])
		return new GameObject(*_objects[name]);
	return nullptr;
}

unsigned int ResourceManager::GetGameObjectUID(const char *name)
{
	if (name == nullptr) return -1;
	if (_counter.find(name) == _counter.end()) {
		_counter[name] = -1;
	}
	_counter[name]++;
	return _counter[name];
}

void ResourceManager::SetTransform(pugi::xml_node node, Transform &T) {
	if (!node)
		return;

	pugi::xml_node prop;

	prop = node.child("position");
	if (prop)
		T.position = glm::ExtractVector<glm::vec3>(prop.text().get());

	prop = node.child("rotation");
	if (prop) {
		glm::vec3 rotation = glm::ExtractVector<glm::vec3>(prop.text().get());
		T.SetRotation(rotation);
	}

	prop = node.child("scale");
	if (prop)
		T.scale = glm::ExtractVector<glm::vec3>(prop.text().get());

	T.Update();
}