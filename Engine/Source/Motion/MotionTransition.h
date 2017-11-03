#pragma once

#include <include/dll_export.h>
#include <Event/EventListener.h>

class Transform;

class DLLExport MotionTransition
	: public EventListener
{
	public:
		MotionTransition(Transform *source, Transform *endState, float duration);
		void StartMotion(Transform *source);
		void StartMotion();

	private:
		void Update();
		void OnEvent(EventType Event, void *data);

	private:
		double duration;
		double startTime;
		float transitionTime;

		Transform *source;
		Transform *startState;
		Transform *endState;
};
