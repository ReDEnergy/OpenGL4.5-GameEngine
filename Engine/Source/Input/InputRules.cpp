#include "InputRules.h"

#include <include/utils.h>

#include <Core/InputSystem.h>

#define NR_MAX_RULES 10

INPUT_RULE InputRules::activeRule = INPUT_RULE::R_GAMEPLAY;
vector<int> InputRules::rulesConfig(NR_MAX_RULES, 0);
vector<INPUT_RULE> InputRules::rules;

void InputRules::Init() {

	int rule = 0;
	SET_BIT(rule, static_cast<uint>(INPUT_GROUP::IG_DEBUG));
	SET_BIT(rule, static_cast<uint>(INPUT_GROUP::IG_EDITOR));
	SET_BIT(rule, static_cast<uint>(INPUT_GROUP::IG_ENGINE));
	SET_BIT(rule, static_cast<uint>(INPUT_GROUP::IG_GAMEPLAY));
	rulesConfig[static_cast<uint>(INPUT_RULE::R_GAMEPLAY)] |= rule;

	rule = 0;
	SET_BIT(rule, static_cast<uint>(INPUT_GROUP::IG_DEBUG));
	SET_BIT(rule, static_cast<uint>(INPUT_GROUP::IG_ENGINE));
	SET_BIT(rule, static_cast<uint>(INPUT_GROUP::IG_IN_GAME_MENU));
	rulesConfig[static_cast<uint>(INPUT_RULE::R_IN_GAME_MENU)] |= rule;

	rule = 0;
	SET_BIT(rule, static_cast<uint>(INPUT_GROUP::IG_DEBUG));
	SET_BIT(rule, static_cast<uint>(INPUT_GROUP::IG_EDITOR));
	SET_BIT(rule, static_cast<uint>(INPUT_GROUP::IG_ENGINE));
	SET_BIT(rule, static_cast<uint>(INPUT_GROUP::IG_DEBUG_MENU));
	rulesConfig[static_cast<uint>(INPUT_RULE::R_EDITOR)] |= rule;

	activeRule = INPUT_RULE::R_GAMEPLAY;
}

void InputRules::PushRule(INPUT_RULE rule)
{
	activeRule = rule;
	rules.push_back(rule);
	//InputSystem::RuleUpdate();
}

void InputRules::PopRule() {
	if (rules.size() > 1) {
		rules.pop_back();
		activeRule = rules.back();
	}
	//InputSystem::RuleUpdate();
}

bool InputRules::IsActiveRule(INPUT_RULE rule) {
	return (activeRule == rule);
}

bool InputRules::IsActiveGroup(INPUT_GROUP group) {
	return (rulesConfig[static_cast<uint>(activeRule)] & 1 << static_cast<uint>(group)) != 0;
}

INPUT_RULE InputRules::GetActiveRule() {
	return activeRule;
}