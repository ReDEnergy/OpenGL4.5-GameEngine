#include "MotionTransition.h"

#include <include/math.h>
#include <include/glm.h>
#include <include/glm_utils.h>

#include <Core/Engine.h>
#include <Component/Transform/Transform.h>

MotionTransition::MotionTransition(Transform *source, Transform * endState, float duration)
{
	this->duration = duration;
	this->endState = endState;
	this->source = source;
	startState = new Transform(*source);
}

void MotionTransition::Update() {

	auto deltaTime = Engine::GetElapsedTime() - startTime;
	float transitionTime = static_cast<float>(deltaTime / duration);

	if (transitionTime < 1) {
		auto position = glm::mix(startState->GetWorldPosition(), endState->GetWorldPosition(), transitionTime);
		auto rotation = glm::slerp(startState->GetWorldRotation(), endState->GetWorldRotation(), transitionTime);
		source->SetWorldPosition(position);
		source->SetWorldRotation(rotation);
	}
	else {
		source->SetWorldRotation(endState->GetWorldRotation());
		source->SetWorldPosition(endState->GetWorldPosition());
		UnsubscribeFrom(EventType::FRAME_START);
	}
}

void MotionTransition::StartMotion()
{
	startTime = Engine::GetElapsedTime();
	SubscribeToEvent(EventType::FRAME_START);
}

void MotionTransition::StartMotion(Transform *source)
{
	startState->Copy(*source);
	StartMotion();
}

void MotionTransition::OnEvent(EventType Event, void * data)
{
	if (EventType::FRAME_START == Event) {
		Update();
	}
}
