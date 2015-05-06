//#include <pch.h>
#include "GameObject.h"

#include <Component/AudioSource.h>
#include <Component/AABB.h>
#include <Component/Mesh.h>
#include <Component/Renderer.h>
#include <Component/Transform.h>
#include <Component/ObjectInput.h>
#include <GPU/Shader.h>

#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>
#include <Manager/ColorPicking.h>

#ifdef PHYSICS_ENGINE
#include <Component/Physics.h>
#include <Manager/PhysicsManager.h>
#endif

GameObject::GameObject(const char *name)
{
	Init();
	if (name) {
		refID = new char[strlen(name)];
		refID = strcpy(refID, name);
	}
	renderer = new Renderer();
	transform = new Transform();
}

GameObject::GameObject(const GameObject &obj) {
	Init();
	refID	= obj.refID;
	mesh	= obj.mesh;
	shader	= obj.shader;
	input	= obj.input;
	renderer = obj.renderer;
	transform = new Transform(*obj.transform);
	SetupAABB();

	#ifdef PHYSICS_ENGINE
	if (obj.physics) {
		physics = new Physics(this);
		physics->body = Manager::Physics->GetCopyOf(obj.physics->body);
	}
	#endif
}

GameObject::~GameObject() {
	Manager::Debug->Remove(this);
}

void GameObject::Init()
{
	colorID = Manager::ColorPick->GetColorUID();
	SetDebugView(true);
}

void GameObject::SetupAABB() {
	if (mesh) {
		aabb = new AABB(this);
		aabb->Update();
	}
}

void GameObject::Update() {
	#ifdef PHYSICS_ENGINE
	if (physics) {
		physics->Update();
	}
	#endif

	// TODO event based position update through Transform
	if (audioSource && audioSource->sound3D) {
		audioSource->SetPosition(transform->position);
	}
}

bool GameObject::ColidesWith(GameObject *object) {
	if (!object->aabb)
		return false;
	return aabb->Overlaps(object->aabb);
}

void GameObject::Render() const {
	if (!mesh || !shader) return;
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->model));
	mesh->Render(shader);
}

void GameObject::Render(const Shader *shader) const {
	if (!mesh) return;
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->model));
	mesh->Render(shader);
}

void GameObject::RenderInstanced(const Shader *shader, unsigned int instances) const
{
	if (!mesh) return;
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->model));
	mesh->RenderInstanced(instances);
}

void GameObject::UseShader(Shader *shader) {
	this->shader = shader;
}

void GameObject::SetDebugView(bool value) {
	debugView = value;
	debugView ? Manager::Debug->Add(this) : Manager::Debug->Remove(this);
}

void GameObject::SetAudioSource(AudioSource *source)
{
	audioSource = source;
	audioSource->SetPosition(transform->position);
}

float GameObject::DistTo(GameObject *object)
{
	glm::vec3 d = transform->position - object->transform->position;
	float d2 = (d.x * d.x) + (d.y * d.y) + (d.z * d.z);
	return sqrt(d2);
}
