// pch.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files

//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
//#include <crtdbg.h>

#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>

#include <functional>

// Threading 
#include <atomic>
#include <mutex>

// TODO: reference additional headers your program requires here

#include <Engine.h>