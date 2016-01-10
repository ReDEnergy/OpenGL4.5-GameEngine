#pragma once

#include <string>
#include <vector>
#include <include/dll_export.h>
#include <include/utils.h>

using namespace std;

class DLLExport ProfileTimer
{
	public:
		ProfileTimer();
		~ProfileTimer();

	public:
		void Start();
		void Stop();
		void Reset();
		void Lap();
		void Print();

		void SetMessage(const char* message);

	private:
		bool active;
		double startTime;
		double stopTime;
		vector<double> laps;
		string message;
};