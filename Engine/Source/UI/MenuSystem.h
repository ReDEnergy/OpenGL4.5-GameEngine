#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#include <include/pugixml.h>

using namespace std;

enum class MenuEntryType {
	ACTION,
	PAGE,
	TOGGLE,
	SLIDER,
	SELECT
};

class Text;

/* General page-menu entry */
class PageEntry {
	public:
		PageEntry(pugi::xml_node &entryXML);
		virtual ~PageEntry() {};
		virtual void Trigger() {};
		virtual void EmitAction();

	public:
		string actionID;
		MenuEntryType type;
		Text *text;
};

/* Sends the specific action message once it is activated */
class ActionEntry : public PageEntry {
	public:	
		ActionEntry(pugi::xml_node &entryXML);
		~ActionEntry();
};

/* An entry that points to a new MenuPage */
class MenuEntry : public PageEntry {
	public:	
		MenuEntry(pugi::xml_node &entryXML);
		~MenuEntry();
};

/* 2 state entry - TRUE / FALSE */
class ToggleEntry : public PageEntry {
	public:	
		ToggleEntry(pugi::xml_node &entryXML);
		~ToggleEntry();

		void Trigger();
		bool GetState();

	private:
		bool state;
};

/* A Menu page that can contain one or more entries */
class MenuPage {
	public: 
		MenuPage(pugi::xml_node &pageXML);
		~MenuPage();
		void Render();

	public:
		string UID;
		string name;
		vector<PageEntry*> entries;
};

class MenuSystem 
{
	public:
		MenuSystem();
		~MenuSystem();

		void Load(const char* file);

	private:
		void ConstructPage(pugi::xml_node &pageXML);

	public:
		unordered_map<string, MenuPage*> pages;
};
