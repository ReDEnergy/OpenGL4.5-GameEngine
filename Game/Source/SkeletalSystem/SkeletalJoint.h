#pragma once
#include <string>
#include <vector>

#include <PCH/pch.h>

using namespace std;

class Transform;
class SkeletalTracking;

class SkeletalJoint
	: public GameObject
{
	public:
		SkeletalJoint(const char* name);
		~SkeletalJoint();

		void RenderSkelet(const Shader * shader) const;
		void RenderBones(const Shader *shader) const;
		void ReadSensorRotationState(SkeletalTracking * tracking);
		void ReadSensorPositionState(SkeletalTracking * tracking);

		void UpdateJointScale();

	private:
		void RenderBone(SkeletalJoint * child) const;

	public:
		glm::ivec2 rotConstraint[3];
		uint sensorTrackingID;
};
