#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <unordered_map>

#include <include/dll_export.h>
#include <include/utils.h>

struct LapInfo
{
	using high_resolution_clock = std::chrono::high_resolution_clock;
	using time_point = high_resolution_clock::time_point;

	enum class IntervalType {
		Processing,
		Idle,
	};

	LapInfo(IntervalType type = IntervalType::Idle) : type(type)
	{
		label = 0;
		time = high_resolution_clock::now();
	}

	LapInfo(unsigned int label) : LapInfo(IntervalType::Processing)
	{
		this->label = label;
	}

	long long operator-(const LapInfo &rhs) const
	{
		return (this->time - rhs.time).count();
	}

	template <typename T>
	double GetDeltaTime(const LapInfo &rhs) const
	{
		return (std::chrono::duration<double, T>(this->time - rhs.time)).count();
	}

	unsigned int label;
	IntervalType type;
	time_point time;
};

class DLLExport ProfilingTimer
{
	using high_resolution_clock = std::chrono::high_resolution_clock;
	using time_point = high_resolution_clock::time_point;

	private:
		std::unordered_map<unsigned long long, std::string> timingValues
		{
			{1, " sec"},
			{1000, " ms"},
			{1'000'000, " us"},
			{1'000'000'000, " ns"}
		};

	public:
		ProfilingTimer();
		~ProfilingTimer();

		void Start();
		void Stop();
		void Pause();
		void Resume();
		void Reset();
		
		bool IsActive() const;

		template <typename T = std::ratio<1, 1>>
		double Lap(unsigned int labelID = 0)
		{
			if (isActive)
			{
				LapInfo lapInfo(labelID);
				auto diff = lapInfo.GetDeltaTime<T>(laps.back());
				laps.emplace_back(lapInfo);
				return diff;
			}
			return 0;
		}

		template <typename T = std::ratio<1, 1>>
		double GetDeltaTime() const
		{
			if (laps.size() > 0)
			{
				return laps.back().GetDeltaTime<T>(laps.front());
			}
			return 0;
		}

		template <typename T = std::ratio<1, 1>>
		void Print()
		{
			size_t size = laps.size();
			double total = 0;

			for (size_t i = 1; i < size; i++)
			{
				double interval = laps[i].GetDeltaTime<T>(laps[i - 1]);
				total += interval;
				std::cout << i << " [" << laps[i].label << "]\tD: " << interval << timingValues[T::den] << "\tT: " << total << timingValues[T::den] << endl;
			}
		}

		template <typename T = std::ratio<1, 1>>
		double GetTypeIntervalTime(LapInfo::IntervalType type) const
		{
			return std::chrono::duration<double, T>(processingTimes[static_cast<int>(type)]).count();
		}

		const std::vector<LapInfo>& GetInfo() const;

	private:
		bool isActive;
		std::vector<LapInfo> laps;
		std::chrono::nanoseconds processingTimes[2];
};
