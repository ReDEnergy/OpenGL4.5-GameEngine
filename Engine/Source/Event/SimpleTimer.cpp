#include "SimpleTimer.h"

#include <Core/Engine.h>
#include <Manager/Manager.h>
#include <Manager/EventSystem.h>
#include <Event/EventType.h>
#include <Event/EventListener.h>


SimpleTimer::SimpleTimer(float duration)
{
	this->duration = duration;
	onExprire.clear();
}

SimpleTimer::~SimpleTimer() {};

void SimpleTimer::Start()
{
	startTime = (float)glfwGetTime();
}

void SimpleTimer::Update()
{
	float delta = (float)glfwGetTime() - startTime;
	if (delta > duration) {
		for (auto &func : onExprire) {
			func();
		}
	}
}

void SimpleTimer::OnExpire(function<void()> func) {
	onExprire.push_back(func);
}