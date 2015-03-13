#pragma once
#include <vector>

using namespace std;


enum InputGroup {
	IG_ENGINE,
	IG_DEBUG,
	IG_EDITOR,
	IG_GAMEPLAY,
	IG_MAIN_MENU,
	IG_DEBUG_MENU,
	IG_IN_GAME_MENU,
};


class InputRules
{
	public:
		static enum Rule {
			R_GAMEPLAY,
			R_IN_GAME_MENU
		};

	private:
		static Rule ACTIVE_RULE;
		static vector<int> rulesConfig;

	public:
		static void Init();
		static void SetRule(Rule RULE);
		static bool IsActiveRule(InputGroup GROUP);
};