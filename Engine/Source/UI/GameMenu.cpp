#include "GameMenu.h"

#include <iostream>
using namespace std;

#include <include/gl.h>
#include <include/glm.h>

#include <Core/Engine.h>
#include <Core/Camera/Camera.h>
#include <Component/Text.h>
#include <Component/Transform/Transform.h>

#include <Event/EventType.h>

#include <GPU/Shader.h>
#include <Input/ObjectControl.h>

#include <Manager/Manager.h>
#include <Manager/AudioManager.h>
#include <Manager/ConfigFile.h>
#include <Manager/ShaderManager.h>
#include <Manager/EventSystem.h>

#include <UI/MenuSystem.h>

using namespace std;

GameMenu::GameMenu()
	: ObjectInput(InputGroup::IG_IN_GAME_MENU)
{
	activePage = Manager::GetMenu()->pages["in_game_menu"];
	activeEntryIndex = 0;

	float aspectRation = (float)Engine::Window->resolution.x / Engine::Window->resolution.y;

	HUDCamera = new Camera();
	HUDCamera->SetPerspective(25.0f, aspectRation, 0.1f, 50.0f);
	//HUDCamera->SetOrthgraphic(10, 10, 0.1f, 50.0f);
	HUDCamera->SetPosition(glm::vec3(0, 0, 10.0f));
	HUDCamera->transform->SetWorldRotation(glm::vec3(0, 0, 0));
	HUDCamera->Update();

	for (auto &page : (Manager::GetMenu()->pages)) {
		SetPageLayout(page.second);
	}

	SubscribeToEvent("resume");
	SubscribeToEvent("exit");
	SubscribeToEvent("toggle-test");
	SubscribeToEvent(EventType::OPEN_GAME_MENU);
}

void GameMenu::SetPageLayout(MenuPage *page) {
	int index = 0;
	float offset = 1.6f;
	float lineOffset = 0.25f;
	float leftOffset = -3.20f; 
	for (auto entry : page->entries) {
		entry->text->transform->SetWorldPosition(entry->text->transform->GetWorldPosition() + glm::vec3(leftOffset, offset, 0));
		offset -= lineOffset;
	}
}

GameMenu::~GameMenu() {
}

void GameMenu::Render() const
{
	if (!InputRules::IsActiveRule(InputRule::R_IN_GAME_MENU))
		return;

	glm::vec3 color = glm::vec3(0.967f, 0.333f, 0.486f);
	glm::vec3 selectColor = glm::vec3(0.967f, 0.873f, 0.486f);

	Shader *shader = Manager::GetShader()->GetShader("font");
	shader->Use();
	HUDCamera->BindProjectionMatrix(shader->loc_projection_matrix);
	HUDCamera->BindViewMatrix(shader->loc_view_matrix);
	glUniform3f(shader->text_color, 0.967f, 0.333f, 0.486f);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	for (unsigned int i = 0; i < activePage->entries.size(); i++) {
		auto text = activePage->entries[i]->text;
		(i == activeEntryIndex) ? text->SetColor(selectColor) : text->SetColor(color);
		text->Render(shader);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void GameMenu::Open() {
	InputRules::PushRule(InputRule::R_IN_GAME_MENU);
}

void GameMenu::Close() {
	InputRules::PopRule();
	Manager::GetEvent()->EmitAsync(EventType::CLOSE_MENU, nullptr);
}

void GameMenu::PreviousPage() {
	if (topPages.size() == 0) {
		Close();
		return;
	}
	auto active = topPages.top();
	activeEntryIndex = active.second;
	activePage = active.first;
	topPages.pop();
}

void GameMenu::OnEvent(const string& eventID, void *data)
{
	if (eventID.compare("resume") == 0) {
		Close();
		return;
	}
	if (eventID.compare("exit") == 0) {
		Engine::Exit();
		return;
	}
}

void GameMenu::OnEvent(EventType Event, void *data)
{
	switch (Event)
	{
		case EventType::OPEN_GAME_MENU:
			Open();
			return;
	}
}

void GameMenu::OnKeyPress(int key, int mods) {
	switch (key)
	{
		case GLFW_KEY_ESCAPE:
			PreviousPage();
			return;

		case GLFW_KEY_DOWN:
			activeEntryIndex++;
			activeEntryIndex %= activePage->entries.size();
			return;
		
		case GLFW_KEY_UP:
			if (activeEntryIndex == 0)
				activeEntryIndex = activePage->entries.size() - 1;
			else
				activeEntryIndex--;
			return;

		case GLFW_KEY_ENTER:
			PageEntry *activeEntry = activePage->entries[activeEntryIndex];
			switch (activeEntry->type)
			{
				case MenuEntryType::TOGGLE:
					activeEntry->Trigger();
					break;
				case MenuEntryType::ACTION:
					Manager::GetEvent()->EmitAsync(activeEntry->actionID.c_str(), nullptr); 
					break;

				case MenuEntryType::PAGE:
					MenuPage *page = Manager::GetMenu()->pages[activeEntry->actionID];
					if (page) {
						topPages.push(make_pair(activePage, activeEntryIndex));
						activePage = page;
						activeEntryIndex = 0;
						cout << "Access menu: " << page->name << endl;
					}
					break;
			}
	}
}