//#include <pch.h>
#include "InputRules.h"

#include <include/utils.h>

#include <Core/InputSystem.h>

#define NR_MAX_RULES 10

InputRule InputRules::ACTIVE_RULE = InputRule::R_GAMEPLAY;
vector <int> InputRules::rulesConfig (NR_MAX_RULES, 0);

void InputRules::Init() {

	int rule = 0;
	SET_BIT(rule, InputGroup::IG_DEBUG);
	SET_BIT(rule, InputGroup::IG_EDITOR);
	SET_BIT(rule, InputGroup::IG_ENGINE);
	SET_BIT(rule, InputGroup::IG_GAMEPLAY);
	rulesConfig[InputRule::R_GAMEPLAY] |= rule;

	rule = 0;
	SET_BIT(rule, InputGroup::IG_DEBUG);
	SET_BIT(rule, InputGroup::IG_ENGINE);
	SET_BIT(rule, InputGroup::IG_IN_GAME_MENU);
	rulesConfig[InputRule::R_IN_GAME_MENU] |= rule;

	rule = 0;
	SET_BIT(rule, InputGroup::IG_DEBUG);
	SET_BIT(rule, InputGroup::IG_EDITOR);
	SET_BIT(rule, InputGroup::IG_ENGINE);
	SET_BIT(rule, InputGroup::IG_DEBUG_MENU);
	rulesConfig[InputRule::R_EDITOR] |= rule;

	ACTIVE_RULE = InputRule::R_GAMEPLAY;
}

void InputRules::SetRule(InputRule RULE) {
	ACTIVE_RULE = RULE;
	InputSystem::RuleUpdate();
}

bool InputRules::IsActiveRule(InputGroup GROUP) {
	return (rulesConfig[ACTIVE_RULE] & 1<< GROUP) != 0;
}