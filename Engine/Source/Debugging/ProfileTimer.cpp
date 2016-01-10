#include "ProfileTimer.h"

#include <iostream>
using namespace std;

#include <include/gl.h>

ProfileTimer::ProfileTimer()
{
	Reset();
}

ProfileTimer::~ProfileTimer()
{
}

void ProfileTimer::Start()
{
	active = true;
	startTime = glfwGetTime();
	laps.push_back(startTime);
}

void ProfileTimer::Stop()
{
	active = false;
	stopTime = glfwGetTime();
	laps.push_back(stopTime);
}

void ProfileTimer::Reset()
{
	active = false;
	startTime = 0;
	stopTime = 0;
	laps.clear();
}

void ProfileTimer::Lap()
{
	laps.push_back(glfwGetTime());
}

void ProfileTimer::Print()
{
	double total = 0;
	uint size = laps.size();

	cout << message << "\n";
	for (uint i = 1; i < size; i++) {
		double interval = laps[i] - laps[i - 1];
		total += interval;
		cout << i << ".\t" << interval << " sec\t" << total << " sec" << endl;
	}
}

void ProfileTimer::SetMessage(const char * message)
{
	this->message = message;
}
