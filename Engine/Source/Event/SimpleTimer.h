#pragma once
#include <list>
#include <functional>

#include <include/dll_export.h>

using namespace std;

class DLLExport SimpleTimer
{
	public:
		SimpleTimer(float duration);
		~SimpleTimer();

		void Start();
		void Update();
		void OnExpire(function<void()> func);

	private:
		float duration;
		float startTime;
		list<function<void()>> onExprire;
};