#pragma once
#include <Editor/Windows/Interface/DockWindow.h>

#include <include/gl.h>

#include <Event/EventListener.h>

class Texture;
class GPUBuffers;
class OpenGLWindow;

class GameWindow
	: public DockWindow,
	public EventListener
{
	public:
		GameWindow();
		virtual ~GameWindow() {};

		void Init();
		void Render();
		void RenderQuad();

	private:
		void InitUI();
		void OnEvent(EventType Event, void *data);

	protected:
		void DockedEvent(bool state);

	private:
		bool useContext;
		OpenGLWindow *qtOpenGLWindow;
		GPUBuffers *buffer;
};
