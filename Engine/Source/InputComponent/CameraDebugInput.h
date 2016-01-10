#pragma once
#include <Component/ObjectInput.h>

using namespace std;

class Camera;

class DLLExport CameraDebugInput : public ObjectInput {
	public:
		CameraDebugInput(Camera *camera);
		void OnInputUpdate(float deltaTime, int mods);
	public:
		Camera *camera;
};

