#include <pch.h>
#include "GameWindow.h"

#include <iostream>

using namespace std;

#include <Editor/Windows/OpenGL/OpenGLWindow.h>
#include <Editor/UI/GUI.h>
#include <Utils/GPU.h>

#include <QOpenGLContext>
#include <QWGLNativeContext>
#include <QLayout>
#include <QKeyEvent>

GameWindow::GameWindow()
{
	setWindowTitle("3D Scene");

	InitUI();
	GUI::Set(QT_INSTACE::_3D_SCENE_WINDOW, (void*) this);

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

void GameWindow::DockedEvent(bool state)
{
	auto res = Engine::Window->GetResolution();
	if (state == true) {
		setFixedSize(Engine::Window->GetResolution().x, Engine::Window->GetResolution().y);
	}
	else {
		setMinimumHeight(Engine::Window->GetResolution().y + 22);
	}
}

void GameWindow::Init()
{
	auto value = qtOpenGLWindow->SetAsCurrentContext();
	auto mesh = Manager::GetResource()->GetMesh("screen-quad");
	buffer = new GPUBuffers();
	*buffer = UtilsGPU::UploadData(mesh->positions, mesh->normals, mesh->texCoords, mesh->indices);

	auto nativeHandle = qtOpenGLWindow->GetContext()->nativeHandle();
	auto windowsNative = nativeHandle.value<QWGLNativeContext>();

	auto sctx = qtOpenGLWindow->GetContext()->shareContext();

	CheckOpenGLError();
}

void GameWindow::Render()
{
	auto FBO = FrameBuffer::GetOffScreenBuffer();
	if (!FBO) return;

	auto value = qtOpenGLWindow->SetAsCurrentContext();
	glGetError();

	//FrameBuffer::Unbind(Engine::Window);
	{
		Shader *screen = Manager::GetShader()->GetShader("screen");
		screen->Use();
		auto resolution = Engine::Window->GetResolution();
		glUniform2i(screen->loc_resolution, resolution.x, resolution.y);
		FBO->BindTexture(0, GL_TEXTURE0);
		RenderQuad();
	}

	qtOpenGLWindow->EndFrame();
	CheckOpenGLError();
}

void GameWindow::RenderQuad()
{
	glBindVertexArray(buffer->VAO);
	glDrawElementsBaseVertex(4, 6, GL_UNSIGNED_SHORT, 0, 0);
	glBindVertexArray(0);
	CheckOpenGLError();
}

void GameWindow::OnEvent(EventType Event, void * data)
{
	if (EventType::FRAME_END == Event) {
		Render();
	}
	//if (EventType::FRAME_SYNC == Event) {
	//	qtOpenGLWindow->SetAsCurrentContext();
	//}
}
