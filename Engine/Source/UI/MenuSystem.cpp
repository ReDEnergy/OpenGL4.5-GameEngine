#include "MenuSystem.h"

#include <include/pugixml.h>

#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>
#include <Manager/EventSystem.h>
#include <Component/Text.h>

#include <iostream>
#include <stdio.h>
#include <assert.h>

using namespace std;

MenuSystem::MenuSystem() {
}

MenuSystem::~MenuSystem() {
}

void MenuSystem::Load(const char* file) {
	Manager::Debug->InitManager("Menu");

	pugi::xml_document *doc = pugi::LoadXML(file);
	pugi::xml_node pagesXML = doc->child("pages");
	for (pugi::xml_node pageXML: pagesXML.children()) {
		MenuPage *page = new MenuPage(pageXML);
		pages[page->UID] = page;
	}

	Menu = new GameMenu();
}

void MenuSystem::RenderMenu() {
	Menu->Render();
}

MenuPage::MenuPage(pugi::xml_node &pageXML)
{
	UID = string(pageXML.child_value("id"));
	name = string(pageXML.child_value("name"));

	PageEntry *entry;
	for (pugi::xml_node entryXML: pageXML.child("entries").children())
	{
		const char* type = entryXML.attribute("type").as_string();
		if (strcmp(type, "action") == 0) {
			entry = new ActionEntry(entryXML);
		}
		if (strcmp(type, "page") == 0) {
			entry = new MenuEntry(entryXML);
		}
		if (strcmp(type, "toggle") == 0) {
			entry = new ToggleEntry(entryXML);
		}
		entries.push_back(entry);
	}
}

PageEntry::PageEntry(pugi::xml_node &entryXML)
{
	const char *name = entryXML.child_value("name");
	text = new Text();
	text->SetText(name);
	actionID = entryXML.child_value("action");
}

void PageEntry::EmitAction() {
}

ActionEntry::ActionEntry(pugi::xml_node &entryXML)
	: PageEntry(entryXML)
{
	type = MenuEntryType::ACTION;
}

ActionEntry::~ActionEntry() {
}

MenuEntry::MenuEntry(pugi::xml_node &entryXML)
	: PageEntry(entryXML)
{
	type = MenuEntryType::PAGE;
}

MenuEntry::~MenuEntry() {
}

ToggleEntry::ToggleEntry(pugi::xml_node &entryXML)
	: PageEntry(entryXML)
{
	type = MenuEntryType::TOGGLE;
	this->state = true;

	const char *value = entryXML.child_value("value");
	if (strcmp(value, "false") == 0) {
		this->state = false;
	}
}

ToggleEntry::~ToggleEntry() {
};

void ToggleEntry::Trigger() {
	state = !state;
	Manager::Event->EmitAsync(actionID.c_str(), this);
}

bool ToggleEntry::GetState() {
	return state;
}
