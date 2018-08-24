#pragma once

#include <Component/ObjectInput.h>

class Transform;

enum class ControlMode {
	Scale,
	Rotate,
	TRANSLATE
};

class DLLExport ObjectControl : public ObjectInput {
	public:
		ObjectControl(Transform *transform);
		void OnInputUpdate(float deltaTime, int mods);
		void OnKeyPress(int key, int mods);

	private:
		Transform *transform;
		ControlMode controlMode;
};