//#include <pch.h>
#include "Light.h"

#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Component/Transform/Transform.h>
#include <Component/Mesh.h>
#include <GPU/Shader.h>

using namespace std;

Light::Light()
	: GameObject("light")
{
	Init();
}

Light::Light(const GameObject &gameObject)
	: GameObject(gameObject)
{
	Init();
}

Light::~Light() {
}

void Light::Init()
{
	mesh = Manager::Resource->GetMesh("sphere");
	bulbSize = glm::vec3(0.3f);
	transform->SetScale(glm::vec3(bulbSize));
	active = true;
	RandomDiffuseColor();
}

void Light::RandomDiffuseColor()
{
	float r = (float(rand()) / RAND_MAX); 
	float g = (float(rand()) / RAND_MAX); 
	float b = (float(rand()) / RAND_MAX); 
	diffuseColor = glm::normalize(glm::vec3(r, g, b));
}

void Light::Render(const Shader *shader) const
{
	transform->SetScale(bulbSize);
	GameObject::Render(shader);
}

void Light::RenderForPicking(const Shader * shader) const
{
	transform->SetScale(bulbSize);
	GameObject::RenderForPicking(shader);
}

void Light::RenderDebug(const Shader *shader) const
{
	glUniform4fv(shader->loc_debug_color, 1, glm::value_ptr(diffuseColor)); 
	Render(shader);
}

void Light::Move(const glm::vec3 dir, float deltaTime)
{
	transform->SetWorldPosition(transform->GetWorldPosition() + glm::normalize(dir) * deltaTime);
}
