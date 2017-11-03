#include "ProfileTimer.h"

#include <iostream>

#include <Core/Engine.h>

using namespace std;

ProfileTimer::ProfileTimer()
{
	Reset();
}

ProfileTimer::~ProfileTimer()
{
}

bool ProfileTimer::IsActive() const
{
	return isActive;
}

float ProfileTimer::GetStartTime() const
{
	return (float)startTime;
}

float ProfileTimer::GetDeltaTime() const
{
	return float(laps.back().time - laps.front().time);
}

void ProfileTimer::Start(const char* label)
{
	isActive = true;
	startTime = Engine::GetElapsedTime();
	laps.push_back(LapInfo(label ? label : "Start", startTime));
}

void ProfileTimer::Stop(const char* label)
{
	if (!isActive) return;
	isActive = false;
	stopTime = Engine::GetElapsedTime();
	laps.push_back(LapInfo(label ? label : "Stop", stopTime));
}

void ProfileTimer::Reset()
{
	isActive = false;
	startTime = 0;
	stopTime = 0;
	laps.clear();
}

void ProfileTimer::Lap(const char* label)
{
	if (!isActive) return;
	laps.push_back(LapInfo(label ? label : "Lap", Engine::GetElapsedTime()));
}

const vector<LapInfo>& ProfileTimer::GetInfo() const
{
	return laps;
}

void ProfileTimer::Print()
{
	double total = 0;
	size_t size = laps.size();

	cout << message << "\n";
	for (size_t i = 1; i < size; i++) {
		double interval = laps[i].time - laps[i - 1].time;
		total += interval;
		cout << i << ".\t" << interval << " sec\t" << total << " sec" << endl;
	}
}

void ProfileTimer::SetMessage(const char * message)
{
	this->message = message;
}
