#include "QtGLFW.h"

#include <iostream>

#include <QGuiApplication>
#include <include/gl.h>

using namespace std;

QtGLFW::QtGLFW()
{
	mouseButtons[Qt::MouseButton::LeftButton] = GLFW_MOUSE_BUTTON_LEFT;
	mouseButtons[Qt::MouseButton::RightButton] = GLFW_MOUSE_BUTTON_RIGHT;
	mouseButtons[Qt::MouseButton::MiddleButton] = GLFW_MOUSE_BUTTON_RIGHT;

	// Map special keys that are different from GLFW

	for (auto i = 0; i < 12; i++) {
		keyboardButtons[Qt::Key::Key_F1 + i] = GLFW_KEY_F1 + i;
	}

	keyboardButtons[Qt::Key::Key_Escape] = GLFW_KEY_ESCAPE;
	keyboardButtons[Qt::Key::Key_Tab] = GLFW_KEY_TAB;

	keyboardButtons[Qt::Key::Key_Backspace] = GLFW_KEY_BACKSPACE;
	keyboardButtons[Qt::Key::Key_Return] = GLFW_KEY_ENTER;
	keyboardButtons[Qt::Key::Key_Enter] = GLFW_KEY_ENTER;

	keyboardButtons[Qt::Key::Key_NumLock] = GLFW_KEY_NUM_LOCK;
	keyboardButtons[Qt::Key::Key_CapsLock] = GLFW_KEY_CAPS_LOCK;
	keyboardButtons[Qt::Key::Key_ScrollLock] = GLFW_KEY_SCROLL_LOCK;

	keyboardButtons[Qt::Key::Key_SysReq] = GLFW_KEY_PRINT_SCREEN;
	keyboardButtons[Qt::Key::Key_Pause] = GLFW_KEY_PAUSE;

	keyboardButtons[Qt::Key::Key_Home] = GLFW_KEY_HOME;
	keyboardButtons[Qt::Key::Key_End] = GLFW_KEY_END;

	keyboardButtons[Qt::Key::Key_PageUp] = GLFW_KEY_PAGE_UP;
	keyboardButtons[Qt::Key::Key_PageDown] = GLFW_KEY_PAGE_DOWN;

	keyboardButtons[Qt::Key::Key_Insert] = GLFW_KEY_INSERT;
	keyboardButtons[Qt::Key::Key_Delete] = GLFW_KEY_DELETE;

	keyboardButtons[Qt::Key::Key_Alt] = GLFW_KEY_LEFT_ALT;
	keyboardButtons[Qt::Key::Key_AltGr] = GLFW_KEY_RIGHT_ALT;
	keyboardButtons[Qt::Key::Key_Control] = GLFW_KEY_LEFT_CONTROL;
	keyboardButtons[Qt::Key::Key_Shift] = GLFW_KEY_LEFT_SHIFT;
	keyboardButtons[Qt::Key::Key_Meta] = GLFW_KEY_LEFT_SUPER;
	keyboardButtons[Qt::Key::Key_Super_L] = GLFW_KEY_LEFT_SUPER;
	keyboardButtons[Qt::Key::Key_Super_R] = GLFW_KEY_RIGHT_SUPER;

	keyboardButtons[Qt::Key::Key_Left] = GLFW_KEY_LEFT;
	keyboardButtons[Qt::Key::Key_Right] = GLFW_KEY_RIGHT;
	keyboardButtons[Qt::Key::Key_Up] = GLFW_KEY_UP;
	keyboardButtons[Qt::Key::Key_Down] = GLFW_KEY_DOWN;
}

int QtGLFW::GetMouseButton(int qtButton)
{
	return mouseButtons[qtButton];
}

int QtGLFW::GetKeyButton(int qtKey)
{
	if (qtKey < 384)
	{
		return qtKey;
	}
	if (keyboardButtons.find(qtKey) != keyboardButtons.end())
		return keyboardButtons.at(qtKey);

	cout << "[WARNING] - KEY BINDING NOT FOUND";
	return 0;
}

int QtGLFW::GetModifiers()
{
	int glfwMod = 0;
	auto modifier = QGuiApplication::keyboardModifiers();
	glfwMod |= (modifier & Qt::KeyboardModifier::AltModifier) ? GLFW_MOD_ALT : 0;
	glfwMod |= (modifier & Qt::KeyboardModifier::ControlModifier) ? GLFW_MOD_CONTROL : 0;
	glfwMod |= (modifier & Qt::KeyboardModifier::ShiftModifier) ? GLFW_MOD_SHIFT : 0;
	glfwMod |= (modifier & Qt::KeyboardModifier::MetaModifier) ? GLFW_MOD_SUPER : 0;
	return glfwMod;
}

