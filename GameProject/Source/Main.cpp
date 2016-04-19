#include <pch.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>

#ifndef OPENGL_ES
#include <Editor/EditorMain.h>
#endif
#include <Game/Game.h>

#undef DLLExport

using namespace std;

int main(int argc, char **argv)
{
	srand((unsigned int)time(NULL));

	Engine::Init();
	Manager::Init();

	Game *game = new Game();
	game->Init();

	Engine::SetWorldInstance(game);

	auto defaultWindowProps = Manager::GetConfig()->windowProperties;
	if (defaultWindowProps->visible) {
		Engine::Run();
	}
	else {
		GameEngine app(argc, argv);
	}

	return 0;
}