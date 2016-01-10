#pragma once
#include <Core/GameObject.h>

class TransitionGate : public GameObject
{
	public:
		TransitionGate(const GameObject &gameObject);
		~TransitionGate();
};

