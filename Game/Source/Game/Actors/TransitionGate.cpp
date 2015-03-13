#include "TransitionGate.h"

#include <Manager/Manager.h>
#include <Component/Physics.h>
#include <Physics/PhysicsPhantom.h>

TransitionGate::TransitionGate(const GameObject &gameObject)
	: GameObject(gameObject)
{
	this->physics = new PhysicsPhantom(this);
}

TransitionGate::~TransitionGate() {
}