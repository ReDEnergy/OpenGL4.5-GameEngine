//#include <pch.h>
#include "SceneManager.h"

#include <iostream>

#ifdef PHYSICS_ENGINE
#include <Component/Physics.h>
#endif
#include <Component/AudioSource.h>
#include <Component/Transform.h>

#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>

#include <Lighting/PointLight.h>

#include <Manager/Manager.h>
#include <Manager/AudioManager.h>
#include <Manager/ResourceManager.h>
#include <Manager/DebugInfo.h>
#include <Manager/EventSystem.h>

SceneManager::SceneManager() {
}

SceneManager::~SceneManager() {
}

void SceneManager::LoadScene(const char *fileName) {
	Manager::Debug->InitManager("Scene");
	sceneFile = fileName;

	frustumObjects.clear();
	activeObjects.clear();
	lights.clear();
	toRemove.clear();

	// Load document
	pugi::xml_document &doc = *pugi::LoadXML(fileName);

	// TODO
	// Load different type of objects based on container
	// examples - lights, coins, etc
	// const char *container = child.name();

	// --- Load GameObjects --- //
	const char *refID;
	pugi::xml_node audioInfo;
	pugi::xml_node transformInfo;
	pugi::xml_node objects = doc.child("objects");

	for (pugi::xml_node obj: objects.children()) {
		refID	= obj.child_value("ref");
		transformInfo = obj.child("transform");
		audioInfo = obj.child("audio");

		GameObject *GO = Manager::Resource->GetGameObject(refID);
		if (GO == nullptr) {
			cout << "Error: Resource game-object not found => '" << refID << "'" << endl;
			continue;
		}
		Manager::Resource->SetTransform(transformInfo, *GO->transform);

		if (audioInfo) {
			bool loop = audioInfo.attribute("loop").as_bool();
			pugi::xml_attribute att = audioInfo.attribute("volume");
			GO->SetAudioSource(Manager::Audio->GetAudioSource(audioInfo.text().get()));
			GO->audioSource->SetLoop(loop);
			if (!att.empty())
				GO->audioSource->SetVolume(att.as_float());
		}

		AddObject(GO);
	}

	// --- Load Lights --- //
	pugi::xml_node pointLights = doc.child("lights");

	for (pugi::xml_node light: pointLights.children()) {
		transformInfo = light.child("transform");

		auto *L = new PointLight();
		Manager::Resource->SetTransform(transformInfo, *L->transform);

		this->lights.push_back(L);
	}

	// --- Load orbs --- //
	Transform *T = new Transform();
	pugi::xml_node orbs = doc.child("orbs");
	for (pugi::xml_node orb: orbs.children()) {
		Manager::Resource->SetTransform(orb, *T);
		Manager::Event->EmitSync(EventType::CREATE_ORB, (Object*)T);
	}
}

// TODO - bug clear Physics World... just loaded objects
void SceneManager::ReloadScene() {
	LoadScene(sceneFile);
}

void SceneManager::Update() {
	bool shouldAdd = !toAdd.empty();
	bool shouldRemove = !toRemove.empty();

	if (shouldAdd || shouldRemove) {
		if (shouldAdd) {
			for (auto obj: toAdd) {
				activeObjects.push_back(obj);
			}
			toAdd.clear();
		}

		if (shouldRemove) {
			for (auto obj: toRemove) {
				auto elem = find(activeObjects.begin(), activeObjects.end(), obj);
				activeObjects.remove(obj);
			}
			toRemove.clear();
		}
	}

	for (auto obj : activeObjects) {
		obj->Update();
	}
}

void SceneManager::AddObject(GameObject *obj) {
	toAdd.push_back(obj);
	#ifdef PHYSICS_ENGINE
	if (obj->physics)
		obj->physics->AddToWorld();
	#endif
}

void SceneManager::RemoveObject(GameObject *obj) {
	toRemove.push_back(obj);
	#ifdef PHYSICS_ENGINE
	if (obj->physics)
		obj->physics->RemoveFromWorld();
	#endif
}

GameObject* SceneManager::GetObject(char *refID, unsigned int instanceID)
{
	for (auto obj : activeObjects) {
		if (obj->instanceID == instanceID &&
			strcmp(obj->refID, refID) == 0)
			return obj;
	}
	return nullptr;
}

void SceneManager::FrustumCulling(Camera *camera) {
	frustumObjects.clear();
	for (auto obj: activeObjects) {
		if (camera->ColidesWith(obj))
			frustumObjects.push_back(obj);
	}
}
