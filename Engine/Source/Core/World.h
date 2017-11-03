#pragma once

class WindowObject;

class World {
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

	public:
		WindowObject *window;

	private:
		double previousTime;
		double elapsedTime;
		double deltaTime;
		bool paused;
		bool shouldClose;
		World *world;

};