#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <Core/Engine.h>
#include <Game/Game.h>
#include <Manager/Manager.h>

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