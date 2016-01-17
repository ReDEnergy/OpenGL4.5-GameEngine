#pragma once

#include <cstdlib>
#include <cstdio>

#include <include/dll_export.h>
#include <include/gl.h>

class WindowObject;
class World;

using namespace std;

/*
 *	Graphic Engine
 */
class DLLExport Engine {
	public:
		static void Init();
		static void Run();
		static void Pause();
		static void Exit();
		static void SetWorldInstance(World *world);

		static float GetLastFrameTime();
		static float GetElapsedTime();

	public:
		static WindowObject *Window;

	private:
		static void Update();
		static void ComputeFrameDeltaTime();

	private:
		static double elapsedTime;
		static float deltaTime;
		static bool paused;
		static World *world;
};

// TODO - Add support for multiple windows
// Each window can have it's own world or share contexts