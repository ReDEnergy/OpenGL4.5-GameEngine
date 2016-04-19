#include "ProfileTimer.h"

#include <iostream>
#include <chrono>

#include <include/gl.h>

//using Time = std::chrono::high_resolution_clock;
using miliseconds = std::chrono::duration<int, std::ratio<1, 1000>>;
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

void ProfileTimer::Start()
{
	isActive = true;
	startTime = glfwGetTime();
	laps.push_back(LapInfo("Start", startTime));
}

void ProfileTimer::Stop()
{
	if (!isActive) return;
	isActive = false;
	stopTime = glfwGetTime();
	laps.push_back(LapInfo("Stop", stopTime));
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
	laps.push_back(LapInfo(label, glfwGetTime()));
}

const vector<LapInfo>& ProfileTimer::GetInfo() const
{
	return laps;
}

void ProfileTimer::Print()
{
	double total = 0;
	uint size = laps.size();

	cout << message << "\n";
	for (uint i = 1; i < size; i++) {
		double interval = laps[i].time - laps[i - 1].time;
		total += interval;
		cout << i << ".\t" << interval << " sec\t" << total << " sec" << endl;
	}
}

void ProfileTimer::SetMessage(const char * message)
{
	this->message = message;
}
