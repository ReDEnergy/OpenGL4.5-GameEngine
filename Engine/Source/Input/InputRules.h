#pragma once
#include <vector>
#include <include/dll_export.h>

using namespace std;

enum class INPUT_GROUP : unsigned int
{
	IG_ENGINE,
	IG_DEBUG,
	IG_EDITOR,
	IG_GAMEPLAY,
	IG_MAIN_MENU,
	IG_DEBUG_MENU,
	IG_IN_GAME_MENU,
};

enum class INPUT_RULE : unsigned int
{
	R_EDITOR,
	R_GAMEPLAY,
	R_IN_GAME_MENU
};

class DLLExport InputRules
{
	private:
		static INPUT_RULE activeRule;
		static vector<int> rulesConfig;
		static vector<INPUT_RULE> rules;

	public:
		static void Init();
		static void PushRule(INPUT_RULE rule);
		static void PopRule();
		static bool IsActiveRule(INPUT_RULE rule);
		static bool IsActiveGroup(INPUT_GROUP group);
		static INPUT_RULE GetActiveRule();
};