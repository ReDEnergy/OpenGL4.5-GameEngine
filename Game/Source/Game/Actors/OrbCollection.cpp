#include "OrbCollection.h"

#include <Component/Transform.h>
#include <Core/GameObject.h>
#include <Game/Orbs/LightOrb.h>
#include <Game/Orbs/PhysicsOrb.h>
#include <Manager/Manager.h>
#include <Manager/EventSystem.h>


OrbCollection::OrbCollection()
{
	collected = 0;
	toCollect = 0;
	SubscribeToEvent(EventType::COLLECT_ORB);
	SubscribeToEvent(EventType::CREATE_ORB);
	SubscribeToEvent(EventType::LEVEL_TRANISTION_GATE);
}

OrbCollection::~OrbCollection() {
}

void OrbCollection::AddOrb(glm::vec3 position) {
	// Light Orb
 	LightOrb *orb = new LightOrb();
	orb->SetPosition(position);
	lightOrbs.push_back(orb);
	toCollect++;
}

void OrbCollection::Collect(LightOrb *orb) {
	collected++;
	auto elem = find(lightOrbs.begin(), lightOrbs.end(), orb);
	lightOrbs.erase(elem);
 	delete orb;

	Manager::Event->EmitSync(EventType::UPDATE_SCORE, this);
}

void OrbCollection::OnEvent(EventType Event, Object *obj) {
	switch (Event)
	{
		case EventType::CREATE_ORB:
			AddOrb((dynamic_cast<Transform*>(obj))->position);
			break;
		case EventType::COLLECT_ORB:
			Collect(dynamic_cast<LightOrb*>(obj));
			break;
		case EventType::LEVEL_TRANISTION_GATE:
			if (collected == toCollect)
				Manager::Event->EmitSync(EventType::LEVEL_FISIHED, nullptr);
			else
				Manager::Event->EmitSync(EventType::WARNING_COLLECT_EVERYTHING, nullptr);
			break;
		default:
			break;
	}
}
