#pragma once

#include <include/dll_export.h>

extern "C"
{
#include <Lua/lua.h>
#include <Lua/lauxlib.h>
#include <Lua/lualib.h>
}

using namespace std;

class DLLExport LuaScripting
{
	public:
		LuaScripting();
		~LuaScripting();
};