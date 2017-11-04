#pragma once
#include <Editor/Windows/Interface/DockWindow.h>
#include <Event/EventListener.h>

class Texture;
class OpenGLWindow;
class GameObject;

class GameWindow
	: public DockWindow,
	public EventListener
{
	public:
		GameWindow();
		virtual ~GameWindow() {};

		void Init();
		void UpdateUI();

	private:
		void InitUI();
		void InitUIState();
		void Render();

		void OnEvent(EventType Event, void *data);

		// Qt events
		void resizeEvent(QResizeEvent * e);

	protected:
		void DockedEvent(bool state);

	private:
		bool useContext;
		OpenGLWindow *qtOpenGLWindow;
		GameObject *renderQuad;
};
