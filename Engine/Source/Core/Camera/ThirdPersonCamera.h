#pragma once
#include <include/glm.h>
#include <include/gl.h>
#include <Core/Camera/Camera.h>

class ThirdPersonCamera : public Camera
{
	public:
		ThirdPersonCamera();
		~ThirdPersonCamera();

		// Rotation
		void RotateOX(float deltaTime);
		void RotateOY(float deltaTime);
		void RotateOZ(float deltaTime);

		// Rotations
		void RotateXTo(float angle);
		void RotateYTo(float angle);
		void RotateZTo(float angle);

		// Zoom
		void MoveCloser(float add);
		void SwitchView(CameraType cameraType);

	public:
		// Distance to target
		float targetDist;
		float targetMaxDist;

		float limitUpTPS;
};