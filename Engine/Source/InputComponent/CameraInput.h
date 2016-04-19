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
		void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods);

	public:
		Camera *camera;
};

