#pragma once
#include <Game/Orbs/LightOrb.h>
#include <Event/EventListener.h>
#include <list>

/*
 * Class GameObject
 */
class OrbCollection : public EventListener
{
	public:
		OrbCollection();
		~OrbCollection();

		void AddOrb(glm::vec3 position);
		void Collect(LightOrb *orb);
		void OnEvent(EventType Event, Object *data);

	public:
		list<LightOrb*> lightOrbs;
		int collected;
		int toCollect;
};
