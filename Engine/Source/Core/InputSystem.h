#pragma once
#include <list>
#include <unordered_map>
#include <array>
#include <string>

#include <include/gl_defines.h>
#include <include/dll_export.h>

class ObjectInput;

class DLLExport InputSystem
{
	private:
		InputSystem();

	public:
		// KeyBoard
		static void KeyCallback(GLFWwindow *W, int key, int scancode, int action, int mods);
		static void Init();

		// Mouse
		static void CursorMove(GLFWwindow *W, double posX, double posY);
		static void MouseClick(GLFWwindow *W, int button, int action, int mods);
		static void MouseScroll(GLFWwindow *W, double offsetX, double offsetY);

	private:
		static void Load(const char* file);
		static void MapKeys();
		static void MapMouse();

	public:
		static std::unordered_map<std::string, bool> eventState;

	private:
		static bool ruleChanged;
		static std::unordered_map<std::string, int> keyMapping;
		static std::unordered_map<std::string, int> mouseMapping;
		static std::string keyInputEvents[16][384][3];
		static std::string mouseInputEvents[16][8][3];
};