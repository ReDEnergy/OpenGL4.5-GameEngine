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
#include <GPU/Texture.h>

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
	sceneOrigin = new GameObject("Scene");

	//frustumObjects.reserve(1000);
	//normalRender.reserve(1000);
	//skinnedRender.reserve(1000);
	//alphaRender.reserve(1000);
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
	pointLights.clear();
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

		this->pointLights.push_back(L);
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
				obj->SetParent(sceneOrigin);
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
			}
			toRemove.clear();
		}
	}

	for (auto &obj : toDelete) {
		SAFE_FREE(obj);
	}
	toDelete.clear();

	sceneOrigin->Update();

	if (activeCamera)
		activeCamera->Update();
}

void SceneManager::AddObject(GameObject *obj)
{
	if (obj == nullptr) return;

	// Ignore command if objects already in scene hierarchy
	auto parent = obj->GetParent();
	while (parent) {
		if (parent == sceneOrigin) {
			sceneOrigin->AddChild(obj);
			return;
		}
		parent = parent->GetParent();
	}

	// Add GameObject to scene map
	auto &list = objectMap[obj->GetName()];
	if (list.size() != 0) {
		cout << "[SceneManager][Warning] GameObject \""<< obj->GetName() << "\" is not unique!" << endl;
	}
	//objectMap[obj->GetName()].push_back(obj);
	list.push_back(obj);

	// Add GameObject to temp list
	toAdd.push_back(obj);

	#ifdef PHYSICS_ENGINE
	//if (obj->physics)
		//obj->physics->AddToWorld();
	#endif
}

void SceneManager::AddPointLight(PointLight * light)
{
	pointLights.push_back(light);
}

void SceneManager::RemoveObject(GameObject * obj, bool destroy)
{
	if (obj == nullptr)
		return;

	// TODO - investigate if this can be treated by the PickerClass
	if (Manager::Picker->GetSelectedObject() == obj) {
		Manager::Picker->ClearSelection();
	}

	if (destroy) {
		toDelete.push_back(obj);
	}

	toRemove.push_back(obj);
	objectMap[obj->GetName()].remove(obj);

#ifdef PHYSICS_ENGINE
	//if (obj->physics)
	//	obj->physics->RemoveFromWorld();
#endif
}

GameObject * SceneManager::GetOrigin() const
{
	return sceneOrigin;
}

void SceneManager::FrameEnded()
{
	sceneOrigin->transform->ClearMotionState();
	activeCamera->transform->ClearMotionState();
}

GameObject* SceneManager::FindGameObject(char * objectName) const
{
	auto it = objectMap.find(objectName);
	if (it != objectMap.end() && it->second.size()) {
		return it->second.front();
	}
	return nullptr;
}

const list<GameObject*>& SceneManager::GetSceneObjects() const
{
	return sceneOrigin->GetChildren();
}

const vector<GameObject*>& SceneManager::GetFrustrumObjects() const
{
	return frustumObjects;
}

const vector<PointLight*>& SceneManager::GetPointLights() const
{
	return pointLights;
}

Camera * SceneManager::GetActiveCamera()
{
	return activeCamera;
}

void SceneManager::SetActiveCamera(Camera * camera)
{
	if (camera == nullptr)
		return;

	activeCamera = camera;
	activeCamera->renderer->SetIsRendered(false);
	activeCamera->SetDebugView(false);
}

void SceneManager::Render(Camera * camera)
{
	// Render simple objects
	{
		R2T->Use();
		camera->BindViewProj(R2T);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		for (auto obj : normalRender)
		{
			obj->Render(R2T);
		}
	}

	// Render transparent objects
	{
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (auto *obj : alphaRender) {
			obj->RenderTransparent(R2T);
		}

		glUniform1f(R2T->loc_transparency, 0.3f);
		for (auto *obj : pointLights) {
			obj->RenderTransparent(R2T);
		}

		glDisable(GL_BLEND);
	}

	// Render Skinned meshes
	if (R2TSk) {
		R2TSk->Use();
		camera->BindViewProj(R2TSk);

		for (auto *obj : skinnedRender) {
			obj->Render(R2TSk);
		}
	}

	for (auto &callback : postRenderCallbacks) {
		callback(*camera);
	}
}

void SceneManager::RenderDeferredLights(Camera *camera, Texture *worldPositions, Texture *worldNormals) const
{
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	Shader *DF = Manager::GetShader()->GetShader("deferred");
	DF->Use();

	camera->BindViewProj(DF);
	glUniform2i(DF->loc_resolution, worldPositions->GetWidth(), worldPositions->GetHeight());

	worldPositions->BindToTextureUnit(GL_TEXTURE0);
	worldNormals->BindToTextureUnit(GL_TEXTURE1);

	for (auto light : pointLights) {
		auto FACE = camera->DistTo(*light) < light->effectRadius ? OpenGL::CULL::FRONT : OpenGL::CULL::BACK;
		light->renderer->SetCulling(FACE);
		light->RenderDeferred(DF);
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void SceneManager::OnPostRender(std::function<void(Camera &camera)> callback)
{
	postRenderCallbacks.push_back(callback);
}

void SceneManager::PrepareObjectForRendering(GameObject * object)
{
	if (object->meshRenderer)
	{
		frustumObjects.push_back(object);
		auto type = object->meshRenderer->mesh->meshType;
		if (type == MeshType::STATIC) {
			if (object->renderer->IsTransparent()) {
				alphaRender.push_back(object);
			}
			else {
				normalRender.push_back(object);
			}
		}
		else if (type == MeshType::SKINNED)
		{
			skinnedRender.push_back(object);
		}
	}
	for (auto &child : object->GetChildren())
	{
		PrepareObjectForRendering(child);
	}
}

void SceneManager::PrepareSceneForRendering(Camera * camera)
{
	frustumObjects.resize(0);
	normalRender.resize(0);
	skinnedRender.resize(0);
	alphaRender.resize(0);
	PrepareObjectForRendering(sceneOrigin);
}

void SceneManager::FrustumCulling(Camera *gameCamera)
{
	PrepareSceneForRendering(gameCamera);
	//frustumObjects.clear();
	//for (auto obj: sceneOrigin->GetChildren()) {
	//		frustumObjects.push_back(obj);
	//}
}

void SceneManager::LightSpaceCulling(Camera * gameCamera, Camera * light)
{
	bool sunMotion = light->transform->GetMotionState();
	// Update Camera BoundingBox if camera has moved or the sun direction is changed

	auto DBG = Manager::GetDebug();
	auto boundingBoxMode = DBG->GetBoundingBoxMode();

	if (boundingBoxMode == DebugInfo::BBOX_MODE::OBJECT_SAPCE) {
		sceneOrigin->aabb->ComputeLocal();
		for (auto obj : sceneOrigin->GetChildren()) {
			if (obj->transform->GetMotionState()) {
				obj->aabb->ComputeLocal();
			}
		}
	}
	else {
		auto lightPosition = light->transform->GetWorldRotation();
		if (sunMotion) {
			for (auto obj : sceneOrigin->GetChildren()) {
				if (obj->aabb) {
					obj->aabb->Update(lightPosition);
				}
			}
		}
		else {
			for (auto obj : sceneOrigin->GetChildren()) {
				if (obj->aabb && obj->transform->GetMotionState()) {
					obj->aabb->Update(lightPosition);
				}
			}
		}
	}

	FrustumCulling(gameCamera);
}

