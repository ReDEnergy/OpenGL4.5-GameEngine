#pragma once
#include "pch.h"

using namespace std;

class SkeletalTracking;
class KinematicBall;
class PitchingMachine;

class KinectPhysics :
	public Prototype
{
	public:
		KinectPhysics();
		~KinectPhysics() {};

		void Init(SkeletalTracking *tracking);

	private:
		void Update();
		void ThrowBall();

		// Listen for mouse and keyboard events
		void OnKeyPress(int key, int mods);

		// Listen for Events
		void OnEvent(EventType Event, void *data);

	private:
		KinematicBall *leftHand;
		KinematicBall *rightHand;
		SkeletalTracking * tracking;
		PitchingMachine *pitchingMachine;
};