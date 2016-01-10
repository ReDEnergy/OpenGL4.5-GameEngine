#include "pch.h"
#include "KinectPhysics.h"

#include <Game/Actors/Physics/PhysicsBall.h>
#include <Game/Actors/Physics/KinematicBall.h>

#include <Kinect/Kinect.h>
#include <Kinect/SkeletalTracking.h>
#include <Kinect/Smoothing/ExponentialJoint.h>
#include <Kinect/Smoothing/ExponentialRotation.h>

#include <SkeletalSystem/SkeletalSystem.h>
#include <SkeletalSystem/SkeletalJoint.h>

#include <Prototyping/PitchingMachine.h>

KinectPhysics::KinectPhysics()
{
	leftHand = new KinematicBall(0.05f);
	leftHand->renderer->SetOpacity(0.3f);
	Manager::GetScene()->AddObject(leftHand);

	rightHand = new KinematicBall(0.05f);
	rightHand->renderer->SetOpacity(0.3f);
	Manager::GetScene()->AddObject(rightHand);

	SubscribeToEvent(EventType::FRAME_UPDATE);

}

void KinectPhysics::Init(SkeletalTracking * tracking)
{
	this->tracking = tracking;
}

void KinectPhysics::ThrowBall()
{
	auto dir = rightHand->transform->GetLocalOYVector();
	auto pos = rightHand->transform->GetWorldPosition() + dir * 0.1f;

	GameObject *obj = new PhysicsBall(0.05f, pos, dir);
	Manager::GetScene()->AddObject(obj);
}

void KinectPhysics::Update()
{
	auto offset = glm::vec3(0, 1.5, 2);

	auto pos = tracking->expJoints[JointType_HandLeft]->predictedPosition;
	auto rot = tracking->expRotations[JointType_HandLeft]->predictedRotation;
	leftHand->SetPositionAndRotation(pos + offset, rot);

	pos = tracking->expJoints[JointType_HandRight]->predictedPosition;
	rot = tracking->expRotations[JointType_HandRight]->predictedRotation;
	rightHand->SetPositionAndRotation(pos + offset, rot);

	if (tracking->rightHandState == HandState::HandState_Open)
		ThrowBall();
}

void KinectPhysics::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_T) {
		ThrowBall();
	}
}

void KinectPhysics::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::FRAME_UPDATE) {
		Update();
	}
}
