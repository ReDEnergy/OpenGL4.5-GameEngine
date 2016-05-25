#include "SceneManager.h"

#include <iostream>
#include <memory>

#include <include/gl.h>

#ifdef PHYSICS_ENGINE
#include <Component/Physics.h>
#endif
#include <Component/AABB.h>
#include <Component/AudioSource.h>
#include <Component/Mesh.h>
#include <Component/MeshRenderer.h>
#include <Component/Renderer.h>
#include <Component/Transform/Transform.h>
#include <GPU/Shader.h>

#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>

#include <Lighting/PointLight.h>
#include <Lighting/DirectionalLight.h>

#include <Manager/Manager.h>
#include <Manager/AudioManager.h>
#include <Manager/DebugInfo.h>
#include <Manager/EventSystem.h>
#include <Manager/ResourceManager.h>
#include <Manager/RenderingSystem.h>
#include <Manager/ShaderManager.h>

#include <UI/ColorPicking/ColorPicking.h>
#include <Utils/Serialization.h>

using namespace std;

SceneManager::SceneManager()
{
	activeCamera = nullptr;
}

SceneManager::~SceneManager() {
}

void SceneManager::Init()
{
	R2T = Manager::Shader->GetShader("rendertargets");
	R2TSk = Manager::Shader->GetShader("r2tskinning");
}

void SceneManager::LoadScene(const char *fileName)
{
	Manager::Debug->InitManager("Scene");
	sceneFile = fileName;

	frustumObjects.clear();
	sceneObjects.clear();
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
		Serialization::ReadTransform(transformInfo, *GO->transform);

		if (audioInfo) {
			bool loop = audioInfo.attribute("loop").as_bool();
			pugi::xml_attribute att = audioInfo.attribute("volume");
			auto audioSource = Manager::Audio->GetAudioSource(audioInfo.text().get());
			if (audioSource) {
				GO->SetAudioSource(audioSource);
				GO->audioSource->SetLoop(loop);
				if (!att.empty())
					GO->audioSource->SetVolume(att.as_float());
			}
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
		Serialization::ReadTransform(transformInfo, *L->transform);

		this->lights.push_back(L);
		AddObject(L);
	}

	Update();
}

// TODO - bug clear Physics World... just loaded objects
void SceneManager::ReloadScene() {
	LoadScene(sceneFile);
}

void SceneManager::GlobalUpdate()
{

}

void SceneManager::Update()
{
	bool shouldAdd = !toAdd.empty();
	bool shouldRemove = !toRemove.empty();

	if (shouldAdd || shouldRemove) {
		if (shouldAdd) {
			for (auto &obj: toAdd) {
				sceneObjects.push_back(obj);
				obj->SetDebugView(true);
			}
			toAdd.clear();
		}

		if (shouldRemove) {
			for (auto &obj: toRemove) {
				obj->SetDebugView(false);
				if (obj->GetParent()) {
					obj->SetParent(nullptr);
				}
				else {
					sceneObjects.remove(obj);
				}
			}
			toRemove.clear();
		}
	}

	for (auto &obj : toDelete) {
		SAFE_FREE(obj);
	}
	toDelete.clear();

	for (auto &obj : sceneObjects) {
		obj->Update();
	}

	if (activeCamera)
		activeCamera->Update();
}

void SceneManager::AddObject(GameObject *obj)
{
	if (obj == nullptr) return;

	toAdd.push_back(obj);
	#ifdef PHYSICS_ENGINE
	if (obj->physics)
		obj->physics->AddToWorld();
	#endif
}

void SceneManager::AddPointLight(PointLight * light)
{
	lights.push_back(light);
}

void SceneManager::RemoveObject(GameObject * obj, bool destroy)
{
	// TODO - investigate if this can be treated by the PickerClass
	if (Manager::Picker->GetSelectedObject() == obj) {
		Manager::Picker->ClearSelection();
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
	for (auto obj : sceneObjects) {
		obj->transform->ClearMotionState();
	}

	activeCamera->transform->ClearMotionState();
}

GameObject * SceneManager::GetGameObject(char * objectName) const
{
	for (auto obj : sceneObjects) {
		if (obj->name.compare(objectName) == 0)
			return obj;
	}
	return nullptr;
}

const list<GameObject*>& SceneManager::GetSceneObjects() const
{
	return sceneObjects;
}

const list<GameObject*>& SceneManager::GetFrustrumObjects() const
{
	return frustumObjects;
}

const vector<PointLight*>& SceneManager::GetPointLights() const
{
	return lights;
}

Camera * SceneManager::GetActiveCamera()
{
	return activeCamera;
}

void SceneManager::SetActiveCamera(Camera * camera)
{
	activeCamera = camera;
}

void SceneManager::Render(Camera * camera)
{
	R2T->Use();
	camera->BindPosition(R2T->loc_eye_pos);
	camera->BindViewMatrix(R2T->loc_view_matrix);
	camera->BindProjectionMatrix(R2T->loc_projection_matrix);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	list<GameObject*> transparentObjects;

	for (auto *obj : frustumObjects) {
		if (obj->meshRenderer && obj->meshRenderer->mesh->meshType == MESH_TYPE::STATIC)
		{
			if (obj->renderer->IsTransparent()) {
				transparentObjects.push_back(obj);
			}
			else {
				obj->Render(R2T);
			}
		}
	}

	// Render transparent objects
	{
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (auto *obj : transparentObjects) {
			if (obj->meshRenderer && obj->meshRenderer->mesh->meshType == MESH_TYPE::STATIC)
			{
				obj->Render(R2T);
			}
		}

		glUniform1f(R2T->loc_transparency, 0.3f);
		for (auto *obj : lights) {
			obj->Render(R2T);
		}

		glDisable(GL_BLEND);
	}

	// Render Skinned meshes
	if (R2TSk) {
		R2TSk->Use();
		camera->BindPosition(R2TSk->loc_eye_pos);
		camera->BindViewMatrix(R2TSk->loc_view_matrix);
		camera->BindProjectionMatrix(R2TSk->loc_projection_matrix);

		for (auto *obj : frustumObjects) {
			if (obj->meshRenderer && obj->meshRenderer->mesh->meshType == MESH_TYPE::SKINNED) {
				obj->Render(R2TSk);
			}
		}
	}

	for (auto &callback : postRenderCallbacks) {
		callback(*camera);
	}
}

void SceneManager::OnPostRender(std::function<void(Camera &camera)> callback)
{
	postRenderCallbacks.push_back(callback);
}

void SceneManager::FrustumCulling(Camera *gameCamera)
{
	frustumObjects.clear();
	for (auto obj: sceneObjects) {
		if (gameCamera->ColidesWith(obj))
			frustumObjects.push_back(obj);
	}
}

void SceneManager::LightSpaceCulling(Camera * gameCamera, Camera * light)
{
	bool sunMotion = light->transform->GetMotionState();
	// Update Camera BoundingBox if camera has moved or the sun direction is changed
	bool change = gameCamera->transform->GetMotionState() || sunMotion;
	if (change) {
		//gameCamera->UpdateBoundingBox(light);
	}

	auto DBG = Manager::GetDebug();
	auto boundingBoxMode = DBG->GetBoundingBoxMode();

	if (boundingBoxMode == DebugInfo::BBOX_MODE::OBJECT_SAPCE) {
		for (auto obj : sceneObjects) {
			if (obj->transform->GetMotionState()) {
				obj->aabb->ComputeLocal();
			}
		}
	}
	else {
		for (auto obj : sceneObjects) {
			if (obj->aabb && (sunMotion || obj->transform->GetMotionState())) {
				obj->aabb->Update(light->transform->GetWorldRotation());
			}
		}
	}

	FrustumCulling(gameCamera);
}
