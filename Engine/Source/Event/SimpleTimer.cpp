#include "SimpleTimer.h"

#include <Core/Engine.h>
#include <Manager/Manager.h>
#include <Manager/EventSystem.h>
#include <Event/EventType.h>
#include <Event/EventListener.h>

using namespace std;

SimpleTimer::SimpleTimer(float duration)
{
	isActive = false;
	this->duration = duration;
	onExprire.clear();
}

SimpleTimer::~SimpleTimer() {};

void SimpleTimer::Start()
{
	isActive = true;
	startTime = Engine::GetElapsedTime();
	SubscribeToEvent(EventType::FRAME_UPDATE);
}

void SimpleTimer::Reset()
{
	isActive = false;
	UnsubscribeFrom(EventType::FRAME_UPDATE);
}

void SimpleTimer::Stop()
{
	isActive = false;
	UnsubscribeFrom(EventType::FRAME_UPDATE);
}

void SimpleTimer::SetDuration(double duration)
{
	this->duration = duration;
}

bool SimpleTimer::IsActive()
{
	return isActive;
}

void SimpleTimer::Update()
{
	auto delta = Engine::GetElapsedTime() - startTime;
	if (delta > duration) {
		for (auto &func : onExprire) {
			func();
		}
		Stop();
	}
}

void SimpleTimer::OnExpire(function<void()> func) {
	onExprire.push_back(func);
}

void SimpleTimer::OnEvent(EventType Event, void * data)
{
	if (EventType::FRAME_UPDATE == Event) {
		Update();
	}
}