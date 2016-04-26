#include "GameObject.h"

#include <include/gl_utils.h>

#include <Component/Animation/AnimationController.h>
#include <Component/AudioSource.h>
#include <Component/AABB.h>
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
#include <Manager/PhysicsManager.h>
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
	referenceName	= obj.referenceName;
	shader			= obj.shader;
	input			= obj.input;
	renderer		= new Renderer(*obj.renderer);
	transform		= new Transform(*obj.transform);

	if (obj.meshRenderer) {
		meshRenderer = new MeshRenderer(*obj.meshRenderer);
		SetupAABB();
	}
	Init();

	#ifdef PHYSICS_ENGINE
	if (obj.physics) {
		physics = new Physics(this);
		physics->body = Manager::Physics->GetCopyOf(obj.physics->body);
	}
	#endif
}

GameObject::~GameObject()
{
	for (auto child : _children) {
		SAFE_FREE(child);
	}
	_children.clear();

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
	input = nullptr;
	meshRenderer = nullptr;
	renderer = nullptr;
	shader = nullptr;
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

	if (meshRenderer && meshRenderer->mesh->meshType == MESH_TYPE::SKINNED)
	{
		animation = new AnimationController();
		animation->Setup((SkinnedMesh*)meshRenderer->mesh);
	}
}

void GameObject::SetMesh(Mesh * mesh)
{
	SAFE_FREE(meshRenderer);
	meshRenderer = new MeshRenderer(*mesh);
	SetupAABB();
}

Mesh* GameObject::GetMesh() const
{
	return meshRenderer ? meshRenderer->mesh : nullptr;
}

void GameObject::SetupAABB()
{
	if (meshRenderer) {
		SAFE_FREE(aabb);
		aabb = new AABB(this);
		aabb->Update();
	}
}

void GameObject::Update()
{
	#ifdef PHYSICS_ENGINE
	if (physics) {
		physics->Update();
	}
	#endif

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

bool GameObject::ColidesWith(GameObject *object)
{
	if (!object->aabb)
		return false;
	return aabb->Overlaps(object->aabb);
}

void GameObject::Render() const
{
	if (!shader) return;
	if (renderer->IsRendered()) {
		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->GetModel()));

		if (meshRenderer) {
			renderer->Use();
			meshRenderer->Render(shader);
		}

		for (auto child : _children) {
			child->Render();
		}
	}
}

void GameObject::Render(const Shader *shader) const
{
	if (renderer->IsRendered()) {
		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->GetModel()));

		auto transparency = renderer->GetOpacity();
		if (transparency != 1 && shader->loc_transparency >= 0) {
			glUniform1ui(shader->loc_object_class, 1);
			glUniform1f(shader->loc_transparency, transparency);
		}
		else {
			glUniform1ui(shader->loc_object_class, 0);
		}

		if (animation) {
			animation->BindSkeletonInfo(shader);
		}

		if (meshRenderer) {
			renderer->Use();
			meshRenderer->Render(shader);
		}
	}

	for (auto child : _children) {
		child->Render(shader);
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
	if (meshRenderer) {
		renderer->Use();
		meshRenderer->RenderDebug(shader);
	}
	if (aabb) {
		Manager::RenderSys->Set(RenderState::WIREFRAME, true);
		aabb->Render(shader);
		Manager::RenderSys->Revert(RenderState::WIREFRAME);
	}
}

void GameObject::RenderForPicking(const Shader * shader) const
{
	GL_Utils::SetColorUnit(shader->loc_debug_color, colorID);
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->GetModel()));

	if (animation) {
		animation->BindSkeletonInfo(shader);
	}

	if (meshRenderer && selectable) {
		renderer->Use();
		meshRenderer->Render(shader);
	}

	for (auto child : _children) {
		child->RenderForPicking(shader);
	}
}

void GameObject::UseShader(Shader *shader)
{
	this->shader = shader;
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
	if (object)
		object->AddChild(this);
}

void GameObject::AddChild(GameObject * object)
{
	if (object->_parent)
		object->_parent->RemoveChild(object);

	object->_parent = this;
	object->transform->SetParent(this->transform);
	this->transform->AddChild(object->transform);
	_children.push_back(object);
}

void GameObject::RemoveChild(GameObject * object)
{
	object->transform->SetParent(nullptr);
	transform->RemoveChild(object->transform);
	_children.remove(object);
}

void GameObject::RemoveChildren()
{
	for (auto child : _children) {
		child->transform->SetParent(nullptr);
		transform->RemoveChild(child->transform);
	}
	_children.clear();
}

GameObject* GameObject::IdentifyByColor(glm::vec3 colorID)
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

void GameObject::SetName(const char * name)
{
	this->name = string(name);
}

const char* GameObject::GetName() const
{
	return name.c_str();
}

float GameObject::DistTo(GameObject *object) const
{
	glm::vec3 d = transform->GetWorldPosition() - object->transform->GetWorldPosition();
	float d2 = (d.x * d.x) + (d.y * d.y) + (d.z * d.z);
	return sqrt(d2);
}

GameObject * GameObject::GetParent() const
{
	return _parent;
}

list<GameObject*> GameObject::GetChildren() const
{
	return _children;
}

unsigned int GameObject::GetNumberOfChildren() const
{
	return _children.size();
}
