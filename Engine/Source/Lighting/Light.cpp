//#include <pch.h>
#include "Light.h"

#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Component/Transform.h>
#include <Component/Mesh.h>
#include <GPU/Shader.h>

using namespace std;

Light::Light()
	: GameObject("light")
{
	light = Manager::Resource->GetGameObject("light-debug");
	transform = light->transform;
	bulbSize = glm::vec3(0.3f);
	active = true;
	RandomDiffuseColor();
}

Light::Light(const GameObject &gameObject)
	: GameObject(gameObject)
{
	light = Manager::Resource->GetGameObject("light-debug");
	light->transform = transform;
	bulbSize = glm::vec3(0.3f);
	active = true;
	RandomDiffuseColor();
}

Light::~Light() {
}

void Light::RandomDiffuseColor() {
	float r = (float(rand()) / RAND_MAX); 
	float g = (float(rand()) / RAND_MAX); 
	float b = (float(rand()) / RAND_MAX); 
	diffuseColor = glm::normalize(glm::vec3(r, g, b));
}

void Light::RenderDebug(const Shader *shader) const {
	glUniform4fv(shader->loc_debug_color, 1, glm::value_ptr(diffuseColor)); 
	glm::vec3 sc = glm::vec3(transform->scale);
	transform->scale = bulbSize;
	transform->Update();
	light->Render(shader);
	transform->scale = sc;
	transform->Update();

}

void Light::Move(const glm::vec3 dir, float deltaTime) {
	transform->position += glm::normalize(dir) * deltaTime;
	transform->Update();
}
