#pragma once
#include <vector>
#include <include/dll_export.h>

using namespace std;


namespace _InputGroup {
	enum E {
		IG_ENGINE,
		IG_DEBUG,
		IG_EDITOR,
		IG_GAMEPLAY,
		IG_MAIN_MENU,
		IG_DEBUG_MENU,
		IG_IN_GAME_MENU,
	};
}

namespace _InputRule {
	enum E {
		R_EDITOR,
		R_GAMEPLAY,
		R_IN_GAME_MENU
	};
}

typedef _InputGroup::E InputGroup;
typedef _InputRule::E InputRule;

class DLLExport InputRules
{
	private:
		static InputRule ACTIVE_RULE;
		static vector<int> rulesConfig;
		static vector<InputRule> rules;

	public:
		static void Init();
		static void PushRule(InputRule RULE);
		static void PopRule();
		static bool IsActiveRule(InputRule RULE);
		static bool IsActiveGroup(InputGroup GROUP);
};