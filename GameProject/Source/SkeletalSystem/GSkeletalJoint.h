#pragma once

class Transform;
class SkeletalTracking;

#include <Core/GameObject.h>

class GSkeletalJoint
	: public GameObject
{
	public:
		GSkeletalJoint(const char* name);
		~GSkeletalJoint();

		void Render(const Shader * shader) const;
		void RenderForPicking(const Shader *shader) const;
		void RenderBones(const Shader *shader) const;
		void ReadSensorRotationState(SkeletalTracking * tracking, bool ignoreNonTracked = false);
		void ReadSensorPositionState(SkeletalTracking * tracking, bool ignoreNonTracked = false);

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
