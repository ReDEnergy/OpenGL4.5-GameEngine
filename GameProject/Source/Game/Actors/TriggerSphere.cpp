#include <pch.h>
#include "TriggerSphere.h"

TriggerSphere::TriggerSphere()
{
	triggered = false;
	auto mesh = Manager::GetResource()->GetMesh("sphere");
	SetMesh(mesh);
}

TriggerSphere::~TriggerSphere()
{
}

void TriggerSphere::AddTriggerColider(GameObject * obj)
{
	colliders.push_back(obj);
}

void TriggerSphere::Update()
{
	for (auto collider : colliders) {
		auto dist = DistTo(*collider);
		if (dist < 0.1f) {
			if (triggered == false) {
				triggered = true;
				triggerFunc();
				return;
			}
			else {
				return;
			}
		}
	}
	triggered = false;
}

void TriggerSphere::OnTrigger(std::function<void()> func)
{
	triggerFunc = func;
}
