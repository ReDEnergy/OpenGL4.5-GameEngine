#include <pch.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <thread>

#ifndef OPENGL_ES
#include <Editor/EditorMain.h>
#endif

#include <Controller/Controller.h>

#undef DLLExport

using namespace std;

int main(int argc, char **argv)
{
	srand((unsigned int)time(NULL));

	Engine::Init();
	Manager::Init();

	GameEngine *app = nullptr;

	auto defaultWindowProps = Manager::GetConfig()->windowProperties;
	if (!defaultWindowProps->visible && !defaultWindowProps->fullScreen) {
		app = new GameEngine(argc, argv);
	}

	Controller::Init();

	auto game = Controller::GetModule("Game");
	game->Resume();

	if (app) {
		app->Start();
	}
	else {
		auto window = WindowManager::GetDefaultWindow();
		window->SetSize(1280, 720);
		window->CenterWindow();
		while (1) { 
			window->PollEvents();
		}
	}

	return 0;
}