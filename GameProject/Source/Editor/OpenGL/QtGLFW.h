#pragma once
#include <unordered_map>
using namespace std;

class QtGLFW
{
	public:
		QtGLFW();

		int GetMouseButton(int qtButton);
		int GetKeyButton(int qtKey);
		int GetModifiers();

	private:
		int mouseButtons[16];
		unordered_map<int, int> keyboardButtons;
};
