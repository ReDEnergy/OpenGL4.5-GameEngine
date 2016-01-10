//#include <pch.h>
#include "InputSystem.h"

#include <Core/Engine.h>
#include <Core/WindowManager.h>
#include <Core/WindowObject.h>

#include <include/glm.h>
#include <include/pugixml.h>

#include <Input/InputRules.h>
#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

#include <Component/ObjectInput.h>

static const int MAX_KEYS = 384;

list<ObjectInput*> InputSystem::observers;
list<ObjectInput*> InputSystem::activeObserver;

unordered_map<string, int> InputSystem::keyMapping;
unordered_map<string, int> InputSystem::mouseMapping;
unordered_map<string, bool> InputSystem::eventState;

string InputSystem::keyInputEvents[16][384][3];
string InputSystem::mouseInputEvents[16][8][3];

bool* InputSystem::keyStates = new bool[MAX_KEYS];
bool* InputSystem::mouseStates = new bool[10];

int InputSystem::scancode = GLFW_KEY_UNKNOWN;
int InputSystem::mods = GLFW_KEY_UNKNOWN;

bool InputSystem::ruleChanged = false;

void InputSystem::KeyCallback(GLFWwindow *W, int key, int scancode, int action, int mods)
{
	keyStates[key] = action ? true : false;
	InputSystem::scancode = scancode;
	InputSystem::mods = mods;
	NotifyObservers(key, action, mods);
	if (action == GLFW_PRESS)
	{
		int currentInputRule = InputRules::GetActiveRule();
		string eventToEmit = keyInputEvents[currentInputRule][key][mods];
		if ( !eventToEmit.empty() ) {
			eventState[eventToEmit] = true;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		int currentInputRule = InputRules::GetActiveRule();
		if (key == GLFW_KEY_LEFT_SHIFT) mods += 1;
		if (key == GLFW_KEY_LEFT_CONTROL) mods += 2;
		if (key == GLFW_KEY_LEFT_ALT) mods += 4;
		string eventToEmit = keyInputEvents[currentInputRule][key][mods];
		if ( !eventToEmit.empty() ) {
			eventState[eventToEmit] = false;
		}
	}
}

bool InputSystem::KeyHold(int keyCode)
{
	return keyStates[keyCode];
}

bool InputSystem::MouseHold(int button)
{
	return mouseStates[button];
}

void InputSystem::Init()
{
	for (int i=0; i<MAX_KEYS; i++)
		keyStates[i] = GLFW_RELEASE;

	for (int i = 0; i < 10; i++)
		mouseStates[i] = GLFW_RELEASE;

	MapKeys();
	MapMouse();
	Load("Config/Input.xml");
}

int InputSystem::GetMods()
{
	return mods;
}

void InputSystem::CursorMove(GLFWwindow *W, double posX, double posY)
{
	WindowObject *Wobj = WindowManager::GetWindowObject(W);

	glm::ivec2 mousePos = glm::ivec2(posX, posY);
	glm::ivec2 delta = mousePos - Wobj->pointerPos;

	NotifyObservers(mousePos.x, mousePos.y, delta.x, delta.y);
	Wobj->SetPointerPosition(mousePos);
}

void InputSystem::MouseClick(GLFWwindow *W, int button, int action, int mods)
{
	double posX, posY;
	glfwGetCursorPos(W, &posX, &posY);
	
	MouseClick(button, action, (int)posX, (int)posY, mods);

	// Event Based Methods

	if (action == GLFW_PRESS)
	{
		int currentInputRule = InputRules::GetActiveRule();
		string eventToEmit = mouseInputEvents[currentInputRule][button][mods];
		if (!eventToEmit.empty()) {
			eventState[eventToEmit] = true;
		}
	}
	else {
		int currentInputRule = InputRules::GetActiveRule();
		if (button == GLFW_KEY_LEFT_SHIFT) mods += 1;
		if (button == GLFW_KEY_LEFT_CONTROL) mods += 2;
		if (button == GLFW_KEY_LEFT_ALT) mods += 4;
		string eventToEmit = mouseInputEvents[currentInputRule][button][mods];
		if (!eventToEmit.empty()) {
			eventState[eventToEmit] = false;
		}
	}
}

void InputSystem::MouseMove(int posX, int posY, int deltaX, int deltaY)
{
	NotifyObservers(posX, posX, deltaX, deltaY);
}

void InputSystem::MouseClick(int button, int action, int posX, int posY, int mods)
{
	InputSystem::mods = mods;
	mouseStates[button] = action ? true : false;
	for (auto obs : activeObserver)
	{
		if (obs->IsActive())
			obs->OnMouseBtnEvent(posX, posY, button, action, mods);
	}
}

void InputSystem::Subscribe(ObjectInput *IC)
{
	observers.push_back(IC);
	if (IC->IsActive())
		activeObserver.push_back(IC);
}

void InputSystem::RemoveSubscription(ObjectInput *IC)
{
	// TODO - a linked list is better for observer not vector
}

void InputSystem::RuleUpdate()
{
	ruleChanged = true;
}

void InputSystem::EndFrame()
{
	if (ruleChanged) {
		activeObserver.clear();
		for (auto obs:observers) {
			if (obs->IsActive())
				activeObserver.push_back(obs);
		}
		ruleChanged = false;
	}
}

void InputSystem::UpdateObservers(float deltaTime)
{
	for (auto obs:activeObserver) {
		if (obs->IsActive())
			obs->OnInputUpdate(deltaTime, mods);
	}
}

void InputSystem::NotifyObservers(int key, int action, int mods)
{
	if (action == GLFW_PRESS) {
		for (auto obs:activeObserver)
			if (obs->IsActive())
				obs->OnKeyPress(key, mods);
		return;
	}
	if (action == GLFW_RELEASE) {
		for (auto obs:activeObserver)
			if (obs->IsActive())
				obs->OnKeyRelease(key, mods);
	}
}

void InputSystem::NotifyObservers(int mouseX, int mouseY, int deltaX, int deltaY)
{
	for (auto obs:activeObserver)
		obs->OnMouseMove(mouseX, mouseY, deltaX, deltaY);
}

void InputSystem::MapKeys()
{
	string s;
	// Mapping between GLFW and engine event keys
	// Basic letter keys
	for (unsigned int i = 0; i < 26; i++)
	{
		s.clear();
		s.push_back('A' + i);
		keyMapping.insert(std::make_pair(s, GLFW_KEY_A + i));
	}

	// Basic number keys
	for (unsigned int i = 0; i < 10; i++)
	{
		s.clear();
		s.push_back('0' + i);
		keyMapping.insert(std::make_pair(s, GLFW_KEY_0 + i));
	}

	// F1-F12 keys
	for (unsigned int i = 0; i < 12; i++)
	{
		s.clear();
		s.push_back('F');
		s.push_back('1' + i);
		keyMapping.insert(std::make_pair(s, GLFW_KEY_F1 + i));
	}

	// Arrow keys
	keyMapping.insert(std::make_pair("UP", GLFW_KEY_UP));
	keyMapping.insert(std::make_pair("DOWN", GLFW_KEY_DOWN));
	keyMapping.insert(std::make_pair("LEFT", GLFW_KEY_LEFT));
	keyMapping.insert(std::make_pair("RIGHT", GLFW_KEY_RIGHT));

	// Other keys
	keyMapping.insert(std::make_pair("ENTER", GLFW_KEY_ENTER));
	keyMapping.insert(std::make_pair("ESCAPE", GLFW_KEY_ESCAPE));
	keyMapping.insert(std::make_pair("SPACE", GLFW_KEY_SPACE));
	keyMapping.insert(std::make_pair("CTRL", GLFW_KEY_LEFT_CONTROL));
	keyMapping.insert(std::make_pair("SHIFT", GLFW_KEY_LEFT_SHIFT));
	keyMapping.insert(std::make_pair("ALT", GLFW_KEY_LEFT_ALT));
	keyMapping.insert(std::make_pair("TAB", GLFW_KEY_TAB));
}

void InputSystem::MapMouse()
{
	// Left, right and middle buttons (same thing as buttons 1,2 and 3)
	mouseMapping.insert(std::make_pair("LEFT", GLFW_MOUSE_BUTTON_LEFT));
	mouseMapping.insert(std::make_pair("RIGHT", GLFW_MOUSE_BUTTON_RIGHT));
	mouseMapping.insert(std::make_pair("MIDDLE", GLFW_MOUSE_BUTTON_MIDDLE));

	// Buttons 4-8
	string s;
	for (unsigned int i = 0; i < 5; i++)
	{
		s.clear();
		s.push_back('4' + i);
		mouseMapping.insert(std::make_pair(s, GLFW_MOUSE_BUTTON_4 + i));
	}
}

void InputSystem::Load(const char* file)
{
	// TODO:
	//  - left control and right control? should find a solution for stuff like that
	pugi::xml_document *doc = pugi::LoadXML(file);
	pugi::xml_node statesXML = doc->child("states");
	int current_state = 0;
	for (pugi::xml_node stateXML : statesXML.children())
	{
		for (pugi::xml_node actionXML : stateXML.children())
		{
			// Get a key or mouse button
			int key = 0, mouse = 0;
			if (actionXML.child("key")) key = keyMapping[actionXML.child_value("key")];
			else mouse = mouseMapping[actionXML.child_value("mouse")];
			
			// Now get the modifiers
			int mods = 0;
			pugi::xml_node specialXML = actionXML.child("special");
			if (specialXML.attribute("SHIFT")) mods += 1;
			if (specialXML.attribute("CTRL")) mods += 2;
			if (specialXML.attribute("ALT")) mods += 4;

			// Special case: the key is a modifier itself. A better solution would be good here.
			if (key) {
				if (key == GLFW_KEY_LEFT_SHIFT) mods += 1;
				if (key == GLFW_KEY_LEFT_CONTROL) mods += 2;
				if (key == GLFW_KEY_LEFT_ALT) mods += 4;
			}

			// Map the key combination to the event and set the event to false initially
			if (key) keyInputEvents[current_state][key][mods] = actionXML.child_value("event");
			else mouseInputEvents[current_state][mouse][mods] = actionXML.child_value("event");
			eventState[actionXML.child_value("event")] = false;
		}
		current_state++;
	}
}