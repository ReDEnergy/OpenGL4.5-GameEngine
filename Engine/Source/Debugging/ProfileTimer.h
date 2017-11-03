#pragma once

#include <string>
#include <vector>
#include <include/dll_export.h>
#include <include/utils.h>

struct LapInfo
{
	LapInfo(std::string label, double time)
		: label(label), time(time) {};

	std::string label;
	double time;
};

class DLLExport ProfileTimer
{
	public:
		ProfileTimer();
		~ProfileTimer();

		void Start(const char* label = "Start");
		void Stop(const char* label = "Stop");
		void Lap(const char* label = "Lap");
		void Reset();
		void Print();

		bool IsActive() const;
		float GetStartTime() const;
		float GetDeltaTime() const;
		const std::vector<LapInfo>& GetInfo() const;

		void SetMessage(const char* message);

	private:
		bool isActive;
		double startTime;
		double stopTime;
		std::vector<LapInfo> laps;
		std::string message;
};