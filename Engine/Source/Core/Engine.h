#pragma once

#include <cstdlib>
#include <cstdio>

#include <include/dll_export.h>
#include <include/gl.h>

#include <Core/WindowObject.h>
#include <Core/World.h>

using namespace std;

/*
 *	Graphic Engine
 */
class DLLExport Engine {
	public:
		static void Init();
		static void Run();
		static void SetWorldInstance(World *world);
		static void Pause();
		static void Exit();

	public:
		static WindowObject *Window;

	private:
		static void Render();
		static void ComputeFrameDeltaTime();

	private:
		static double elapsedTime;
		static double deltaTime;
		static bool paused;
		static World *world;
};

// TODO - Add support for multiple windows
// Each window can have it's own world or share contexts