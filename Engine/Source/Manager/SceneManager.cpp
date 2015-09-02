#include "SceneManager.h"

#include <iostream>

#ifdef PHYSICS_ENGINE
#include <Component/Physics.h>
#endif
#include <Component/AABB.h>
#include <Component/AudioSource.h>
#include <Component/Transform/Transform.h>

#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>

#include <Lighting/PointLight.h>
#include <Lighting/DirectionalLight.h>

#include <Manager/Manager.h>
#include <Manager/AudioManager.h>
#include <Manager/ResourceManager.h>
#include <Manager/DebugInfo.h>
#include <Manager/EventSystem.h>
#include <UI/ColorPicking/ColorPicking.h>

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
		auto area = light.child("area").text().as_float();

		auto *L = new PointLight();
		L->SetArea(area);
		Manager::Resource->SetTransform(transformInfo, *L->light->transform);

		this->lights.push_back(L);
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
				obj->SetDebugView(true);
			}
			toAdd.clear();
		}

		if (shouldRemove) {
			for (auto obj: toRemove) {
				activeObjects.remove(obj);
				obj->SetDebugView(false);
			}
			toRemove.clear();
		}
	}

	for (auto obj : toDelete) {
		SAFE_FREE(obj);
	}
	toDelete.clear();

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

void SceneManager::RemoveObject(GameObject * obj, bool destroy)
{
	if (Manager::Picker->GetSelectedObject() == obj) {
		Manager::Picker->SetSelectedObject(nullptr);
	}

	if (destroy) {
		toDelete.push_back(obj);
	}

	toRemove.push_back(obj);

#ifdef PHYSICS_ENGINE
	if (obj->physics)
		obj->physics->RemoveFromWorld();
#endif
}

void SceneManager::FrameEnded()
{
	for (auto obj : activeObjects) {
		obj->transform->ClearMotionState();
	}

	activeCamera->transform->ClearMotionState();
}

GameObject* SceneManager::GetGameObject(char *refID, unsigned int instanceID)
{
	for (auto obj : activeObjects) {
		if (obj->instanceID == instanceID &&
			strcmp(obj->referenceName, refID) == 0)
			return obj;
	}
	return nullptr;
}

Camera * SceneManager::GetActiveCamera()
{
	return activeCamera;
}

void SceneManager::SetActiveCamera(Camera * camera)
{
	activeCamera = camera;
}

void SceneManager::FrustumCulling(Camera *camera) {
	frustumObjects.clear();
	for (auto obj: activeObjects) {
		if (camera->ColidesWith(obj))
			frustumObjects.push_back(obj);
	}
}

void SceneManager::LightSpaceCulling(Camera * camera, DirectionalLight * light)
{
	camera->UpdateBoundingBox(light);
	bool sunMotion = light->transform->GetMotionState();
	for (auto obj : activeObjects) {
		if (obj->aabb && (sunMotion || obj->transform->GetMotionState())) {
			obj->aabb->Update(light->transform->GetWorldRotation());
		}
	}
	FrustumCulling(camera);
}
