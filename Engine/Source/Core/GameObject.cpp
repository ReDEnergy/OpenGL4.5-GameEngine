#include "GameObject.h"

#include <include/gl_utils.h>

#include <Component/Animation/AnimationController.h>
#include <Component/AudioSource.h>
#include <Component/AABB.h>
#include <Component/GameScript.h>
#include <Component/Mesh.h>
#include <Component/MeshRenderer.h>
#include <Component/Renderer.h>
#include <Component/Text.h>
#include <Component/Transform/Transform.h>
#include <Component/ObjectInput.h>
#include <GPU/Shader.h>

#include <Rendering/DirectOpenGL.h>

#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>
#include <Manager/ColorManager.h>
#include <Manager/ResourceManager.h>
#include <Manager/RenderingSystem.h>
#include <UI/ColorPicking/ColorPicking.h>

#include <Utils/OpenGL.h>

#ifdef PHYSICS_ENGINE
#include <Component/Physics.h>
#include <Physics/PhysicsManager.h>
#endif

using namespace std;

GameObject::GameObject()
{
	Clear();
	referenceName = nullptr;
	renderer = new Renderer();
	transform = new Transform();
	Init();
}

GameObject::GameObject(const char *referenceObject)
{
	Clear();
	if (referenceObject) {
		referenceName = referenceObject;
		name = referenceObject;
	}
	renderer = new Renderer();
	transform = new Transform();
	Init();
}

// TODO
// Copy game hierarchy also

GameObject::GameObject(const GameObject &obj)
{
	Clear();
	name			= obj.name;
	referenceName	= obj.referenceName;
	renderer		= new Renderer(*obj.renderer);
	transform		= new Transform(*obj.transform);

	if (obj.meshRenderer) {
		meshRenderer = new MeshRenderer(*obj.meshRenderer);
		SetupAABB();
	}
	Init();

	#ifdef PHYSICS_ENGINE
//	physics = new Physics(this);
	#endif
}

GameObject::~GameObject()
{
	for (auto child : _children) {
		SAFE_FREE(child);
	}
	_children.clear();

	for (auto script : _scripts) {
		SAFE_FREE(script);
	}

	Manager::Debug->Remove(this);
	SAFE_FREE(transform);
	SAFE_FREE(renderer);
}

void GameObject::Clear()
{
	_parent = nullptr;

	aabb = nullptr;
	animation = nullptr;
	audioSource = nullptr;
	meshRenderer = nullptr;
	renderer = nullptr;
	transform = nullptr;
	#ifdef PHYSICS_ENGINE
	physics = nullptr;
	#endif
}

void GameObject::Init()
{
	selectable = true;
	debugView = false;
	instanceID = Manager::Resource->GetGameObjectUID(referenceName);
	colorID = Manager::Color->GetColorUID(this);

	if (meshRenderer && meshRenderer->mesh->meshType == MeshType::SKINNED)
	{
		animation = new AnimationController();
		animation->Setup((SkinnedMesh*)meshRenderer->mesh);
		animation->SetAnimation((uint)0);
	}
}

void GameObject::SetMesh(Mesh * mesh)
{
	SAFE_FREE(meshRenderer);
	meshRenderer = new MeshRenderer(*mesh);
	SetupAABB();
}

const Mesh* GameObject::GetMesh() const
{
	return meshRenderer ? meshRenderer->mesh : nullptr;
}

void GameObject::SetupAABB()
{
	if (meshRenderer) {
		SAFE_FREE(aabb);
		aabb = new AABB(this);
		aabb->Update(glm::quat());
	}
}

void GameObject::Update()
{
	#ifdef PHYSICS_ENGINE
	if (physics) {
		physics->Update();
	}
	#endif

	for (auto &script : _scripts) {
		script->Update();
	}

	if (animation)
		animation->Update();

	// TODO event based position update through Transform
	if (audioSource && audioSource->sound3D) {
		audioSource->SetPosition(transform->GetWorldPosition());
	}

	for (auto child : _children) {
		child->Update();
	}
}

bool GameObject::ColidesWith(const GameObject &object) const
{
	if (!object.aabb)
		return false;
	return aabb->Overlaps(object.aabb);
}

void GameObject::Render(const Shader *shader) const
{
	if (renderer->IsRendered() && meshRenderer && meshRenderer->IsValid()) {
		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->GetModel()));
		glUniform1ui(shader->loc_object_class, 0);

		if (animation) {
			animation->BindSkeletonInfo(shader);
		}

		renderer->Use();
		meshRenderer->Render();
	}
}

void GameObject::RenderTransparent(const Shader * shader) const
{
	if (renderer->IsRendered() && meshRenderer && meshRenderer->IsValid()) {
		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->GetModel()));

		auto opacity = renderer->GetOpacity();
		glUniform1ui(shader->loc_object_class, 1);
		glUniform1f(shader->loc_transparency, opacity);

		renderer->Use();
		meshRenderer->Render();
	}
}

void GameObject::RenderInstanced(const Shader *shader, unsigned int instances) const
{
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->GetModel()));
	if (meshRenderer) {
		renderer->Use();
		meshRenderer->RenderInstanced(instances);
	}
}

void GameObject::RenderDebug(const Shader * shader) const
{
	//if (meshRenderer) {
	//	renderer->Use();
	//}
}

void GameObject::RenderForPicking(const Shader * shader) const
{
	GL_Utils::SetColorUnit(shader->loc_debug_color, colorID);
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->GetModel()));

	if (animation) {
		animation->BindSkeletonInfo(shader);
	}

	if (selectable)
	{
		if (meshRenderer) {
			renderer->Use();
			meshRenderer->Render();
		}
	}
}

void GameObject::LogDebugInfo() const
{
}

glm::vec3 GameObject::GetColorID() const
{
	return colorID;
}

void GameObject::SetParent(GameObject *object)
{
	if (_parent)
		_parent->RemoveChild(this);
	if (object) {
		object->AddChild(this);
	}
}

void GameObject::AddChild(GameObject * object)
{
	if (object->_parent)
		object->_parent->RemoveChild(object);

	object->_parent = this;
	transform->AddChild(object->transform);
	_children.push_back(object);
}

void GameObject::RemoveChild(GameObject * object)
{
	object->_parent = nullptr;
	transform->RemoveChild(object->transform);
	_children.remove(object);
}

void GameObject::RemoveChildren()
{
	for (auto child : _children) {
		transform->RemoveChild(child->transform);
	}
	_children.clear();
}

GameObject* GameObject::IdentifyByColor(const glm::vec3 &colorID)
{
	if (this->colorID == colorID) {
		return (selectable ? this : nullptr);
	}

	for (auto child : _children) {
		auto *obj = child->IdentifyByColor(colorID);
		if (obj) {
			return obj;
		}
	}

	return nullptr;
}

void GameObject::SetSelectable(bool value) {
	selectable = value;
}

void GameObject::SetDebugView(bool value)
{
	debugView = value;
	debugView ? Manager::Debug->Add(this) : Manager::Debug->Remove(this);
}

void GameObject::SetAudioSource(AudioSource *source)
{
	audioSource = source;
	audioSource->SetPosition(transform->GetWorldPosition());
}

void GameObject::SetName(std::string name)
{
	this->name = std::move(name);
}

const char* GameObject::GetName() const
{
	return name.c_str();
}

float GameObject::DistTo(const GameObject &object) const
{
	return transform->DistanceTo(object.transform);
}

GameObject * GameObject::GetParent() const
{
	return _parent;
}

const list<GameObject*>& GameObject::GetChildren() const
{
	return _children;
}

unsigned int GameObject::GetNumberOfChildren() const
{
	return static_cast<unsigned int>(_children.size());
}

void GameObject::AddScript(GameScript *script)
{
	if (script)
	{
		script->AttachTo(this);
		_scripts.push_back(script);
	}
}

void GameObject::RemoveScript(GameScript *script)
{
	if (script) {
		script->Detach();
		_scripts.remove(script);
	}
}

const std::list<GameScript*>& GameObject::GetScripts() const
{
	return _scripts;
}

void GameObject::TriggerEnter(GameObject * object)
{
	if (!object) return;

	for (auto script : _scripts) {
		script->OnTriggerEnter(object);
	}
}

void GameObject::TriggerExit(GameObject * object)
{
	for (auto script : _scripts) {
		script->OnTriggerExit(object);
	}
}
