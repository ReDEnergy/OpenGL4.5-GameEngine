#pragma once
#include "InputRules.h"

#define SET_BIT(item, bit) (item) |= (1 << (bit))
#define CLEAR_BIT(item, bit) (item) &= ~(1 << (bit))

InputRules::Rule InputRules::ACTIVE_RULE = InputRules::R_GAMEPLAY;
vector <int> InputRules::rulesConfig (10, 1 << IG_ENGINE);

void InputRules::Init() {

	int rule = 0;
	SET_BIT(rule, InputGroup::IG_DEBUG);
	SET_BIT(rule, InputGroup::IG_GAMEPLAY);
	rulesConfig[Rule::R_GAMEPLAY] |= rule;

	rule = 0;
	SET_BIT(rule, InputGroup::IG_IN_GAME_MENU);
	rulesConfig[Rule::R_IN_GAME_MENU] |= rule;

	ACTIVE_RULE = Rule::R_GAMEPLAY;
}

void InputRules::SetRule(Rule RULE) {
	ACTIVE_RULE = RULE;
}

bool InputRules::IsActiveRule(InputGroup GROUP) {
	return (rulesConfig[ACTIVE_RULE] & 1<< GROUP) != 0;
}