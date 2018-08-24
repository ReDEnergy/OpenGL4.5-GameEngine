#pragma once
#include <include/dll_export.h>

#include <Component/ObjectInput.h>

class Camera;

class DLLExport CameraInput : public ObjectInput
{
	public:
		CameraInput(Camera *camera);
		void OnInputUpdate(float deltaTime, int mods);
		void OnKeyPress(int key, int mods);
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods);
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods);

	private:
		Camera *camera;
		float sensitivityX;
		float sensitivityY;
};

