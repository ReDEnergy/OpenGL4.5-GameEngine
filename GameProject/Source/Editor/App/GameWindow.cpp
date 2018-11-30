#include <pch.h>
#include "GameWindow.h"

#include <iostream>

using namespace std;

#include <Editor/Windows/OpenGL/OpenGLWindow.h>
#include <Editor/UI/DockWindowManager.h>
#include <Utils/GPU.h>

#include <Game/Game.h>

#include <QOpenGLContext>
#include <QWGLNativeContext>
#include <QLayout>
#include <QKeyEvent>

GameWindow::GameWindow()
{
	setWindowTitle("3D Scene");

	InitUI();

	LoadStyleSheet("game-window.css");
	SubscribeToEvent(EventType::FRAME_END);
	SubscribeToEvent(EventType::FRAME_SYNC);
}

void GameWindow::InitUI()
{
	qtOpenGLWindow = new OpenGLWindow();
	qtOpenGLWindow->Init(WindowManager::GetDefaultWindow());
	qtLayout->addWidget(qtOpenGLWindow->container);
}

void GameWindow::InitUIState()
{
	auto FBO = Game::GetOffScreenBuffer();
	auto resolution = FBO->GetResolution();
	auto aspectRatio = static_cast<float>(resolution.x) / resolution.y;

	resize(height() * aspectRatio, height());
}

void GameWindow::DockedEvent(bool state)
{
	//auto res = Engine::Window->GetResolution();
	//if (state == true) {
	//	setFixedSize(Engine::Window->GetResolution().x, Engine::Window->GetResolution().y);
	//}
	//else {
	//	setMinimumHeight(Engine::Window->GetResolution().y + 22);
	//}
}

void GameWindow::Init()
{
	auto value = qtOpenGLWindow->SetAsCurrentContext();
	CheckOpenGLError();

	renderQuad = Manager::GetResource()->GetGameObject("render-quad");
	renderQuad->meshRenderer->InitForNewContext();

	auto nativeHandle = qtOpenGLWindow->GetContext()->nativeHandle();
	auto windowsNative = nativeHandle.value<QWGLNativeContext>();

	auto sctx = qtOpenGLWindow->GetContext()->shareContext();

	CheckOpenGLError();
}

void GameWindow::UpdateUI()
{
	Render();
}

void GameWindow::Render()
{
	auto FBO = Game::GetOffScreenBuffer();
	if (!FBO) return;

	// Compute the correct viewport based on original texture aspect ratio
	// TODO - do this only on-resize
	auto resolution = FBO->GetResolution();
	auto aspectRatio = static_cast<float>(resolution.x) / resolution.y;

	auto width = qtOpenGLWindow->width();
	auto height = qtOpenGLWindow->height();

	// TODO - should also consider aspect ratio of the Qt window
	// There should be 4 cases based on the aspect ration of the original texture and the output window
	
	if (aspectRatio > 1) {
		auto diffWidth = height * aspectRatio - width;
		auto offsetLeft = diffWidth / 2;
		qtOpenGLWindow->SetViewport(-offsetLeft, 0, width + diffWidth, height);
	}
	else {
		auto diffHeight = width - height * aspectRatio;
		auto offsetTop = aspectRatio * diffHeight / 2;
		qtOpenGLWindow->SetViewport(0, -offsetTop, width, height + 2 * offsetTop);
	}

	OPENGL_RAII_LOCK();
	auto value = qtOpenGLWindow->SetAsCurrentContext();
	CheckOpenGLError();

	Shader *screen = Manager::GetShader()->GetShader("screen");
	screen->Use();
	FBO->BindTexture(0, GL_TEXTURE0);
	renderQuad->Render(screen);

	qtOpenGLWindow->EndFrame();
	CheckOpenGLError();
}

void GameWindow::OnEvent(EventType Event, void * data)
{
	if (EventType::FRAME_END == Event) {
		//Render();
	}
	//if (EventType::FRAME_SYNC == Event) {
	//	qtOpenGLWindow->SetAsCurrentContext();
	//}
}

void GameWindow::resizeEvent(QResizeEvent * e)
{
	cout << "[Window] [Resize] " << e->size().width() << ", "<< e->size().height() << endl;
}
