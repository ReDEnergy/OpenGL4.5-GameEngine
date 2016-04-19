#include "ThirdPersonCamera.h"

#include <include/math.h>

#include <Component/Transform/Transform.h>
#include <Component/Mesh.h>


ThirdPersonCamera::ThirdPersonCamera()
	: GameObject("3rd-camera")
{
	targetDist = 20.f;
	targetMaxDist = 200.0f;	
	limitUpTPS =  0.95f * (float) M_PI / 2;
}

ThirdPersonCamera::~ThirdPersonCamera() {

}

void ThirdPersonCamera::RotateOX(float deltaTime) {
}

void ThirdPersonCamera::RotateOY(float deltaTime) {
}

void ThirdPersonCamera::RotateOZ(float deltaTime) {

}

void ThirdPersonCamera::RotateXTo(float angle)
{
}

void ThirdPersonCamera::RotateYTo(float angle)
{
}

void ThirdPersonCamera::RotateZTo(float angle)
{
}

void ThirdPersonCamera::MoveCloser(float add) {
	if (type == CameraType::FirstPerson)	
		return;

	if (targetDist + add <= 0.5f)
		return;

	if (targetDist + add > targetMaxDist)
		return;

	targetDist += add;
	transform->SetWorldPosition(transform->GetWorldPosition() + transform->GetLocalOZVector() * add);
}

void ThirdPersonCamera::SwitchView(CameraType cameraType)
{
	// Switch to FirstPerson
	if (type == CameraType::ThirdPerson) {
		type = CameraType::FirstPerson;
		transform->SetWorldPosition(transform->GetWorldPosition() - transform->GetLocalOZVector() * targetDist);
		UpdatePitch(-transform->GetRotationEuler().x);
		return;
	}
}
