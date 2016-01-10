#include "pch.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <Game/Game.h>

#undef DLLExport

using namespace std;

 int main() {

	srand((unsigned int)time(NULL));

	Engine::Init();
	Manager::Init();

	Game *game = new Game();
	game->Init();

	Engine::SetWorldInstance(game);
	Engine::Run();

	return 0;
}