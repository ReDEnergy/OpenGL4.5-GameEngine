#pragma once

#include <include/dll_export.h>

class WindowObject;

class DLLExport World
{
	public:
		World();
		virtual ~World() {};
		virtual void Init();
		virtual void FrameStart() {};
		virtual void Update(float deltaTime) {};
		virtual void FrameEnd() {};

		virtual void Run() final;
		virtual void Pause() final;
		virtual void Exit() final;

		virtual double GetLastFrameTime() final;

	private:
		void LoopUpdate();
		void ComputeFrameDeltaTime();

	protected:
		WindowObject *window;

	private:
		double previousTime;
		double elapsedTime;
		double deltaTime;
		bool paused;
};