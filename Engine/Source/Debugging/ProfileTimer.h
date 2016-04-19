#pragma once

#include <string>
#include <vector>
#include <include/dll_export.h>
#include <include/utils.h>

struct LapInfo
{
	LapInfo(const char* label, double time)
		: label(label), time(time) {};

	const char* label;
	double time;
};

class DLLExport ProfileTimer
{
	public:
		ProfileTimer();
		~ProfileTimer();

	public:
		void Start();
		void Stop();
		void Reset();
		void Lap(const char* label = nullptr);
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