#include "GameScript.h"

#include <Core/GameObject.h>

GameScript::GameScript()
{
	_updateFunc = nullptr;
}

GameScript::~GameScript()
{
	Detach();
}

void GameScript::AttachTo(GameObject * object)
{
	GO = object;
}

void GameScript::Detach()
{
	GO = nullptr;
}

void GameScript::OnUpdate(std::function<void()> updateFunc)
{
	this->_updateFunc = updateFunc;
}

void GameScript::Update()
{
	if (_updateFunc)
		_updateFunc();
	FrameUpdate();
}