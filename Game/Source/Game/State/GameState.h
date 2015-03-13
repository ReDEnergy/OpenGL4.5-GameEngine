#pragma once

using namespace std;


enum class RunState {
	WORLD_EDITOR,
	GAMEPLAY,
	PAUSE_MENU,
	MAIN_MENU
};

class RuntimeState
{
	public:
		static void Init();

		static RunState STATE;
};