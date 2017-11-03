#pragma once

#include <include/dll_export.h>

/*
 *	Graphic Engine
 */
class DLLExport Engine
{
	public:
		static void Init();
		static void Exit();
		static double GetElapsedTime();
};