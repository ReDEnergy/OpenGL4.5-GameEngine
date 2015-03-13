#include "LightOrb.h"
#include <Manager/Manager.h>
#include <Manager/EventSystem.h>
#include <Manager/AudioManager.h>
#include <Game/Orbs/PhysicsOrb.h>
#include <Component/Physics.h>
#include <Component/Transform.h>

LightOrb::LightOrb()
{
	int scale = rand() % 5 + 6;
	this->transform->scale = glm::vec3(scale);
	this->transform->Update();
	this->physics = new PhysicsOrb(this);
	collect = 0;
}

LightOrb::~LightOrb() {
	physics->RemoveFromWorld();
}

void LightOrb::Update() {
	GameObject::Update();
}

void LightOrb::CollectOrb() {
	Manager::Event->EmitSync(EventType::COLLECT_ORB, this);
	Manager::Audio->PlaySoundFX("bell");
}

void LightOrb::SetPosition(glm::vec3 pos) {
	physics->SetPosition(pos);
	((PhysicsOrb*)physics)->basePos = glm::vec3(pos);
}

