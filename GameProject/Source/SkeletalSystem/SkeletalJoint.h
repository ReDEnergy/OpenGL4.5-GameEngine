#pragma once
#include <string>
#include <vector>

using namespace std;

class Transform;
class SkeletalTracking;

class SkeletalJoint
	: public GameObject
{
	public:
		SkeletalJoint(const char* name);
		~SkeletalJoint();

		void Render(const Shader * shader) const;
		void RenderBones(const Shader *shader) const;
		void ReadSensorRotationState(SkeletalTracking * tracking, bool ignoreNonTracked = false);
		void ReadSensorPositionState(SkeletalTracking * tracking, bool ignoreNonTracked = false);

		void SetJointID(uint jointID);
		void Update();

		void LogDebugInfo();

	private:
		void RenderBone(SkeletalJoint * child) const;
		void RenderConstraint(const Shader *shader) const;

	private:
		Transform *boneOrientation;
		GameObject *constrained;
		uint sensorJointID;
};
