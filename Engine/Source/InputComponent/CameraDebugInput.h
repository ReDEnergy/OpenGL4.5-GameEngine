#pragma once
#include <Component/ObjectInput.h>

using namespace std;

class Camera;

class DLLExport CameraDebugInput : public ObjectInput {
	public:
		CameraDebugInput(Camera *camera);
		void Update(float deltaTime, int mods);
	public:
		Camera *camera;
};

