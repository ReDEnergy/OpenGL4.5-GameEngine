#pragma once

class Transform;
class KinectSkeletalTracking;

#include <Core/GameObject.h>

class GSkeletalJoint
	: public GameObject
{
	public:
		GSkeletalJoint(const char* name);
		~GSkeletalJoint();

		void Render(const Shader * shader) const;
		void RenderForPicking(const Shader *shader) const;
		void RenderSkeleton(Camera & camera) const;
		void RenderBones() const;

#ifdef KINECT_SENSOR
		void ReadSensorRotationState(KinectSkeletalTracking * tracking, bool ignoreNonTracked = false);
		void ReadSensorPositionState(KinectSkeletalTracking * tracking, bool ignoreNonTracked = false);
#endif

		void SetJointID(uint jointID);
		void Update();
		uint GetJointID() const;

		void LogDebugInfo();

	private:
		void RenderBone(GSkeletalJoint * child) const;
		void RenderConstraint(const Shader *shader) const;

	private:
		Transform *boneOrientation;
		GameObject *constrained;
		uint sensorJointID;
};
