#pragma once
#include <unordered_map>

class QtGLFW
{
	public:
		QtGLFW();

		int GetMouseButton(int qtButton);
		int GetKeyButton(int qtKey);
		int GetModifiers();

	private:
		int mouseButtons[16];
		std::unordered_map<int, int> keyboardButtons;
};
