#pragma once
#include <include/dll_export.h>

#include <Component/ObjectInput.h>
using namespace std;

class Camera;

class DLLExport CameraInput : public ObjectInput {
	public:
		CameraInput(Camera *camera);
		void Update(float deltaTime, int mods);
		void OnKeyPress(int key, int mods);
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
	public:
		Camera *camera;
};

