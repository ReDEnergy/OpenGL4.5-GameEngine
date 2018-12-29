#include "ProfilingTimer.h"

#include <iostream>
#include <chrono>
#include <stdio.h>
#include <string.h>

using namespace std;
using namespace std::chrono;

ProfilingTimer::ProfilingTimer()
{
	Reset();
}

ProfilingTimer::~ProfilingTimer()
{
}

bool ProfilingTimer::IsActive() const
{
	return isActive;
}

void ProfilingTimer::Start()
{
	if (isActive == false)
	{
		Reset();
		isActive = true;
		laps.push_back(LapInfo(LapInfo::IntervalType::Processing));
	}
}

void ProfilingTimer::Stop()
{
	if (isActive)
	{
		isActive = false;
		laps.push_back(LapInfo(LapInfo::IntervalType::Idle));

		memset(processingTimes, 0, 2 * sizeof(nanoseconds));

		size_t size = laps.size();
		for (size_t i = 1; i < size; i++)
		{
			int type = static_cast<int>(laps[i - 1].type);
			processingTimes[type] += laps[i].time - laps[i - 1].time;
		}
	}
}

void ProfilingTimer::Reset()
{
	isActive = false;
	laps.clear();
}

void ProfilingTimer::Pause()
{
	laps.push_back(LapInfo(LapInfo::IntervalType::Idle));
}

void ProfilingTimer::Resume()
{
	laps.push_back(LapInfo(LapInfo::IntervalType::Processing));
}

const vector<LapInfo>& ProfilingTimer::GetInfo() const
{
	return laps;
}
