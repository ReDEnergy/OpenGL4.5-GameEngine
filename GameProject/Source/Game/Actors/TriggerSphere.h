#pragma once
#include <list>
#include <functional>
#include <Core/GameObject.h>

class TriggerSphere : public GameObject
{
	public:
		TriggerSphere();
		~TriggerSphere();

		void AddTriggerColider(GameObject *obj);
		void Update();
		void OnTrigger(std::function<void()> func);

	private:
		bool triggered;
		std::list<GameObject*> colliders;
		std::function<void()> triggerFunc;
};
