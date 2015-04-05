#pragma once
#include <stack>
#include <utility>

#include <Component/ObjectInput.h>
#include <Event/EventListener.h>

class MenuPage;
class Camera;

class GameMenu
	: public ObjectInput,
	public EventListener
{
	public:
		GameMenu();
		virtual ~GameMenu();

		void SetPageLayout(MenuPage *page);
		void PreviousPage();
		void Open();
		void Close();
		
		// Input Events
		void Update(float deltaTime, int mods) {};
		void OnKeyPress(int key, int mods);
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {};

		// Custom Events
		void OnEvent(const char *eventID, Object *data);

		void Render() const;

	private:
		char* name;
		Camera *HUDCamera;
		MenuPage *activePage;
		unsigned int activeEntryIndex;
		std::stack<std::pair<MenuPage*, int>> topPages;
};

